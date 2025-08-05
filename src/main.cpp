#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
// #include "isrg_root_x1.h"  // TLS certificate


#include "wifi_handler.h"
#include "mqtt_handler.h"
#include "fsm_device.h"
#include "pinmap.h"


void setup() {
  // Initialize Serial for debugging
  // This is important for debugging and seeing output in the Serial Monitor  
  Serial.begin(115200);
  Serial.println("Starting Smart Medicine Reminder...");
  initPins();
  
  digitalWrite(LED_BUILTIN, LOW); // Turn off LED initially
  for(int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn on LED
    delay(100);
    digitalWrite(LED_BUILTIN, LOW); // Turn off LED
    delay(100);
  } 
  //delay(1000); // Give time for Serial Monitor to open
  // Initialize all hardware
 


  connectToWiFi();
  setupMQTT();
  Serial.println("✅ WiFi and MQTT setup complete!");
  FSMDevice::init();
  
}

void loop() {


  mqttLoop();

  publishStatusPeriodically();
  //toggleLEDNonBlocking(LED_BUILTIN, 500); // Toggle LED every 500ms
  commandConfirmationBlink(LED_BUILTIN, &cmdConfirmFlag); // Blink LED for command confirmation
  //FSMDevice::update();     // Update FSM state machine

  // // User acknowledgment button
  // if (digitalRead(12) == HIGH) {
  //   FSMDevice::handleUserAcknowledge();
  //   delay(300); // debounce
  // }
}
