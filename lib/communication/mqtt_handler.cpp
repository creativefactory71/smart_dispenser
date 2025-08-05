// ========== mqtt_handler.cpp ==========

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "mqtt_handler.h"
#include "smart_reminder.h"
#include "fsm_device.h"
#include "isrg_root_x1.h"
#include <time.h>
#include <ArduinoJson.h>
#include <Arduino.h>



// ========== BOX LOGIC ==========
// This struct should match your dashboard's data view
#define MAX_BOXES 8
struct Box {
  int box_number;
  String medicine_name;
  int stock;
};
Box boxes[MAX_BOXES];
int boxes_count = 0;

// --- Time management ---
struct tm device_base_tm;
unsigned long device_time_millis = 0;

// --- MQTT constants ---
const char* mqtt_server   = "4ec197ecc26c4407bf46382f28e63fc3.s1.eu.hivemq.cloud";
const int   mqtt_port     = 8883;
const char* mqtt_user     = "creativefactory";
const char* mqtt_password = "Creative24";

// --- Topics ---
const char* topic_schedule   = "care/device/abc123/schedule";
const char* topic_config     = "care/device/abc123/config";
const char* topic_refill     = "care/device/abc123/refill";
const char* topic_status     = "care/device/abc123/status";
const char* topic_config_ack = "care/device/abc123/config/ack";
const char* topic_refill_ack = "care/device/abc123/refill/ack";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);
volatile bool cmdConfirmFlag = false;

unsigned long lastStatus = 0;

// --- Utility: Get ISO8601 timestamp (from system time or RTC) ---
String getISOTimestamp() {
  time_t now = time(nullptr);
  struct tm* t = gmtime(&now);
  char buf[30];
   snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02dZ", 
            t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  return String(buf);
}

// --- Publish Functions ---
void publishStatus(const SmartMessage& msg, bool retained = true, int qos = 1) {
  String json = SmartMedicineReminder::toJson(msg);
  client.publish(topic_status, json.c_str(), retained);
  // Pretty-print JSON to Serial
  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, json) == DeserializationError::Ok) {
    serializeJsonPretty(doc, Serial);
    Serial.println();
  } else {
    Serial.println("❌ Failed to pretty-print JSON status.");
  }
}

void publishConfigAck(const SmartMessage& msg, bool retained = true, int qos = 1) {
  String json = SmartMedicineReminder::toJson(msg);
  client.publish(topic_config_ack, json.c_str(), retained);
  Serial.println("✅ Published to /config/ack:\n" + json);
}

void publishRefillAck(const SmartMessage& msg, bool retained = false, int qos = 1) {
  String json = SmartMedicineReminder::toJson(msg);
  client.publish(topic_refill_ack, json.c_str(), retained);
  Serial.println("✅ Published to /refill/ack:\n" + json);
}

void publishErrorStatus(const String& error_type, const String& details) {
  SmartMessage errorMsg;
  errorMsg.header.sender       = "esp32";
  errorMsg.header.device_id    = "abc123";
  errorMsg.header.message_type = "status";
  errorMsg.header.mode         = "error";
 // errorMsg.header.timestamp    = //getISOTimestamp();
  errorMsg.header.version      = "1.0";
  errorMsg.payload.status.mode        = "error";
  errorMsg.payload.status.result      = "error";
  errorMsg.payload.status.error_type  = error_type;
  errorMsg.payload.status.details     = details;
 // errorMsg.payload.status.timestamp   = getISOTimestamp();
  publishStatus(errorMsg, true, 1);
}

// --- MQTT callback ---
void callback(char* topic, byte* payload, unsigned int length) {
  String incomingBuffer = "";
  for (unsigned int i = 0; i < length; i++) incomingBuffer += (char)payload[i];

  Serial.printf("Received MQTT Topic: %s\n", topic);
  Serial.println("Received JSON:");
  Serial.println(incomingBuffer);

  SmartMessage msg = SmartMedicineReminder::parseFromJson(incomingBuffer);

  // --- Topic-based routing ---
  if (strcmp(topic, topic_schedule) == 0 && msg.header.message_type == "schedule") {
    Serial.println("🟢 Handling SCHEDULE message");
    // FSMDevice::processSchedule(msg.payload.schedule, msg.payload.schedule_count);

  } else if (strcmp(topic, topic_config) == 0 && msg.header.message_type == "config") {
    Serial.println("🟢 Handling CONFIG message");
    //setDeviceClock(msg.payload.config.device_time);
    SmartMessage ack;
    ack.header = msg.header;
    ack.header.sender  = "esp32";
    ack.header.message_type = "config_ack";
    ack.header.mode    = "config_ack";
    ack.header.version = "1.0";
    ack.payload.status.result    = "config_applied";
    ack.payload.status.timestamp = getISOTimestamp();
    publishConfigAck(ack);

  } else if (strcmp(topic, topic_refill) == 0 && msg.header.message_type == "refill") {
    Serial.println("🟢 Handling REFILL message");

    int box_number = msg.payload.refill.box_number;
    String med_name = msg.payload.refill.medicine_name;
    int qty = msg.payload.refill.quantity;

    // --- Process the refill ---
    bool found = false;
    for (int i = 0; i < boxes_count; ++i) {
      if (boxes[i].box_number == box_number) {
        boxes[i].stock = qty; // replace or += qty for additive logic
        boxes[i].medicine_name = med_name;
        found = true;
        break;
      }
    }
    if (!found && boxes_count < MAX_BOXES) {
      boxes[boxes_count].box_number = box_number;
      boxes[boxes_count].medicine_name = med_name;
      boxes[boxes_count].stock = qty;
      boxes_count++;
    }

    // (Optional) Send an ACK message
    SmartMessage ack;
    ack.header = msg.header;
    ack.header.sender = "esp32";
    ack.header.message_type = "refill_ack";
    ack.header.mode = "refill_ack";
    ack.header.version = "1.0";
    // Add what you want to acknowledge (optional)
    publishRefillAck(ack);

    Serial.printf("Refill handled: Box %d, %s, Qty %d\n", box_number, med_name.c_str(), qty);

  } else {
    Serial.println("❌ Unknown or invalid topic/message_type, sending error status");
    publishErrorStatus("invalid_topic", "Message received on unexpected topic or with invalid type");
    return;
  }

  Serial.println("✅ Done handling SmartMessage.\n");
  cmdConfirmFlag = true;
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_schedule);
      client.subscribe(topic_config);
      client.subscribe(topic_refill);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setupMQTT() {
  secureClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void mqttLoop() {
  if (!client.connected()) reconnect();
  client.loop();
}

void publishStatusPeriodically() {
  if (millis() - lastStatus > 10000) {
    SmartMessage statusMsg;
    statusMsg.header.sender = "esp32";
    statusMsg.header.device_id = "abc123";
    statusMsg.header.message_type = "status";
    statusMsg.header.mode = "operation";
    statusMsg.header.timestamp = getISOTimestamp();
    statusMsg.header.version = "1.0";

    statusMsg.payload.status.mode = "operation";
    statusMsg.payload.status.result = "success";
    statusMsg.payload.status.schedule_id = "sched_0001";
    statusMsg.payload.status.battery_percent = random(20, 100); // Simulate battery level
    statusMsg.payload.status.timestamp = getISOTimestamp();

    // --- Simulate random pill stock for 3 boxes ---
    int boxes = 5;
    for (int i = 0; i < boxes; i++) {
      int box_num = i + 1;
      int stock = random(0, 11); // 0 to 10 pills
      statusMsg.payload.status.stock_remaining[i][0] = box_num;
      statusMsg.payload.status.stock_remaining[i][1] = stock;
    }
    statusMsg.payload.status.stock_remaining_count = boxes;

    // --- Simulate next schedule info ---
    // Add these new fields to your struct if not already present:
    statusMsg.payload.status.next_time = "10:30";           // Example
    statusMsg.payload.status.next_medicine = "Paracetamol"; // Example

    // If you want it to be random, use:
    const char* meds[] = {"Paracetamol", "Metformin", "Aspirin"};
    int med_idx = random(0, 3);
    statusMsg.payload.status.next_time = String(random(6,23)) + ":" + String(random(0,59));
    statusMsg.payload.status.next_medicine = meds[med_idx];

    publishStatus(statusMsg, true, 1);
    lastStatus = millis();
  }
}


void setDeviceClock(String iso8601) {
  int yyyy, MM, dd, hh, mm, ss;
  if (sscanf(iso8601.c_str(), "%d-%d-%dT%d:%d:%d", &yyyy, &MM, &dd, &hh, &mm, &ss) == 6) {
    device_base_tm.tm_year = yyyy - 1900;
    device_base_tm.tm_mon  = MM - 1;
    device_base_tm.tm_mday = dd;
    device_base_tm.tm_hour = hh;
    device_base_tm.tm_min  = mm;
    device_base_tm.tm_sec  = ss;
    device_time_millis = millis();
  }
}

void getCurrentDeviceTime(struct tm* now_tm) {
  time_t base = mktime(&device_base_tm);
  time_t now = base + ((millis() - device_time_millis) / 1000);
  *now_tm = *localtime(&now);
}
