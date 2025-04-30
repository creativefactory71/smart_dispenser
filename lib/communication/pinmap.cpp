#include <Arduino.h>
#include "pinmap.h"

void initPins() {
  pinMode(DISPENSER_MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(USER_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("✅ Device hardware initialized.");
  delay(1000); // Allow time for setup
}
