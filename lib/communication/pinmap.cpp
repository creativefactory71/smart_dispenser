#include <Arduino.h>
#include "pinmap.h"


void initPins() {
  pinMode(DISPENSER_MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(USER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize built-in LED for status indication
  Serial.println("✅ Device hardware initialized.");
  delay(1000); // Allow time for setup
}
// Non-blocking LED toggle using millis()
void toggleLEDNonBlocking(int pin, unsigned long interval) {
  static unsigned long lastToggleTime = 0; // Local to the function, persists across calls
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastToggleTime >= interval) {
    lastToggleTime = currentMillis;
    digitalWrite(pin, !digitalRead(pin));
  }
}

// Non-blocking 5x blink for command confirmation
void commandConfirmationBlink(int pin, volatile bool* flag) {
  static unsigned long lastMillis = 0;
  static int blinkCount = 0;
  static bool ledState = LOW;
  const int totalBlinks = 5 * 2; // 5 on+off cycles = 10 toggles
  const unsigned long blinkInterval = 50; // 50 ms per toggle

  if (*flag) {
    unsigned long now = millis();
    if (now - lastMillis >= blinkInterval) {
      lastMillis = now;
      // Toggle LED
      ledState = !ledState;
      digitalWrite(pin, ledState);

      blinkCount++;
      if (blinkCount >= totalBlinks) {
        // Done blinking: reset everything
        digitalWrite(pin, LOW);
        *flag = false;
        blinkCount = 0;
        ledState = LOW;
      }
    }
  } else {
    // Always keep LED OFF if flag is not set
    digitalWrite(pin, LOW);
    blinkCount = 0;
    ledState = LOW;
    lastMillis = millis(); // Optional: avoid initial delay on next trigger
  }
}