// ========== mqtt_handler.cpp ==========
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <smart_reminder.h>
#include "isrg_root_x1.h"

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

unsigned long lastStatus = 0;

const char* mqtt_server = "4ec197ecc26c4407bf46382f28e63fc3.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "creativefactory";
const char* mqtt_password = "Creative24";

void callback(char* topic, byte* payload, unsigned int length) {
  String incomingBuffer = "";
  for (unsigned int i = 0; i < length; i++) {
    incomingBuffer += (char)payload[i];
  }

  Serial.println("Received JSON:");
  Serial.println(incomingBuffer);

  SmartMessage msg = SmartMedicineReminder::parseFromJson(incomingBuffer);
  if (msg.payload.schedule_count > 0) {
    Serial.println("Received medicine:");
    Serial.println(msg.payload.schedule[0].medicine_name);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("care/device/abc123/schedule");
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
      statusMsg.header.sender = "care_001";
      statusMsg.header.device_id = "abc123";
      statusMsg.header.message_type = "status";
      statusMsg.header.mode = "operation";
      statusMsg.header.timestamp = "2025-04-10T08:00:00Z";
      statusMsg.header.version = "1.0";
  
      statusMsg.payload.status.mode = "operation";
      statusMsg.payload.status.result = "success";
      statusMsg.payload.status.schedule_id = "sched_0001";
      statusMsg.payload.status.battery = 87;
      statusMsg.payload.status.timestamp = "2025-04-10T08:00:05Z";
  
      statusMsg.payload.status.stock_remaining[0] = {1, 5};
      statusMsg.payload.status.stock_remaining_count = 1;
  
      String json = SmartMedicineReminder::toJson(statusMsg);
      if (client.publish("care/device/abc123/status", json.c_str())) {
        Serial.println("✅ Published to HiveMQ!");
      } else {
        Serial.println("❌ Publish failed!");
      }
  
      Serial.println(json);
      lastStatus = millis();
    }
  }
  