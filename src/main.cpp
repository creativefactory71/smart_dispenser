#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
// #include "isrg_root_x1.h"  // TLS certificate


#include "wifi_handler.h"
#include "mqtt_handler.h"
#include "fsm_device.h"
// #include "pinmap.h"
// #include "time_sync.h"

// #include "communication.h" // Include the header file for SmartMedicineReminder

// const char* ssid = "Creative Factory";
// const char* password = "creative24";

// const char* mqtt_server = "4ec197ecc26c4407bf46382f28e63fc3.s1.eu.hivemq.cloud";
// const int mqtt_port = 8883;
// const char* mqtt_user = "creativefactory";
// const char* mqtt_password = "Creative24";




// WiFiClientSecure secureClient;
// PubSubClient client(secureClient);

// String incomingBuffer;

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.printf("Message received [%s]:\n", topic);

//   // Convert to String
//   incomingBuffer = "";
//   for (unsigned int i = 0; i < length; i++) {
//     incomingBuffer += (char)payload[i];
//   }

//   Serial.println("Received JSON:");
//   Serial.println(incomingBuffer);

//   // Parse JSON using SmartMedicineReminder
//   SmartMessage msg = SmartMedicineReminder::parseFromJson(incomingBuffer);
//   Serial.println("Medicine name in first schedule:");
//   if (!msg.payload.schedule.empty()) {
//     Serial.println(msg.payload.schedule[0].medicine_name);
//   }
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Connecting to MQTT...");
//     if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
//       Serial.println("connected");
//       client.subscribe("care/device/abc123/schedule");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.println(client.state());
//       delay(5000);
//     }
//   }
// }

void setup() {
  // Serial.begin(115200);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print(".");
  //   delay(1000);
  // }

  // secureClient.setCACert(root_ca);
  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);




  // pinMode(DISPENSER_MOTOR_PIN, OUTPUT);
  // pinMode(BUZZER_PIN, OUTPUT);
  // pinMode(USER_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Starting Smart Medicine Reminder...");
  delay(1000); // Give time for Serial Monitor to open
  // Initialize all hardware
  // initPins();


  connectToWiFi();
  FSMDevice::init();
  setupMQTT();
}

void loop() {
  // if (!client.connected()) reconnect();
  // client.loop();

  // // Simulate status update every 10s
  // static unsigned long lastStatus = 0;
  // if (millis() - lastStatus > 10000) { //10s
  //   SmartMessage statusMsg;
  //   statusMsg.header = {
  //     "care_001",
  //     "abc123",
  //     "status",
  //     "operation",
  //     "2025-04-10T08:00:00Z",
  //     "1.0"
  //   };
  //   statusMsg.payload.status = {
  //     "operation",
  //     "success",
  //     "sched_0001",
  //     {{1, 5}}, // stock_remaining: box 1 has 5 left
  //     87,
  //     "2025-04-10T08:00:05Z"
  //   };

  //   String json = SmartMedicineReminder::toJson(statusMsg);
  //   // String json = "testing publish";
  //   Serial.print("JSON length: ");
  //   Serial.println(json.length());
  //   if (client.publish("care/device/abc123/status", json.c_str())) {
  //       Serial.println("✅ Published to HiveMQ!");
  //     } else {
  //       Serial.println("❌ Publish failed!");
  //     }
      
  //   // Serial.println("Published device status:");
  //   Serial.println(json);

  //   lastStatus = millis();
  // }




  mqttLoop();

 publishStatusPeriodically();

  FSMDevice::update();     // Update FSM state machine

  // User acknowledgment button
  if (digitalRead(12) == LOW) {
    FSMDevice::handleUserAcknowledge();
    delay(300); // debounce
  }
}
