#include "device_operation.h"
#include "pinmap.h"

static int stock[16] = {0};  // Default 16 boxes; index: 0 = box 1

namespace DeviceOperations {

static bool buzzerActive = false;
static unsigned long buzzerStartTime = 0;
static int buzzerDuration = 0;

void startBuzzerAsync(int duration_ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerActive = true;
  buzzerStartTime = millis();
  buzzerDuration = duration_ms;
}

void updateBuzzer() {
  if (buzzerActive && (millis() - buzzerStartTime > buzzerDuration)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
  }
}


  void initHardware() {
    // pinMode(DISPENSER_MOTOR_PIN, OUTPUT);
    // pinMode(BUZZER_PIN, OUTPUT);
    // digitalWrite(DISPENSER_MOTOR_PIN, LOW);
    // digitalWrite(BUZZER_PIN, LOW);
    Serial.println("✅ Device hardware initialized.");
  }

  void triggerDispense(int box_id, int count) {
    Serial.printf("🚚 Dispensing %d pills from box %d...\n", count, box_id);
    
    // Simulate dispensing action
    for (int i = 0; i < count; i++) {
      digitalWrite(DISPENSER_MOTOR_PIN, HIGH);
      delay(500); // Simulated time for 1 pill
      digitalWrite(DISPENSER_MOTOR_PIN, LOW);
      delay(200);
    }

    stock[box_id - 1] = max(0, stock[box_id - 1] - count);
    Serial.printf("📦 Stock remaining in box %d: %d\n", box_id, stock[box_id - 1]);
  }

  void startBuzzer(int duration_ms) {
    Serial.println("🔔 Buzzer ON");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration_ms);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("🔕 Buzzer OFF");
  }

  void stopBuzzer() {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("🔕 Buzzer stopped manually");
  }

  void enterRefillMode() {
    Serial.println("🧪 Entering Refill Mode...");
    for (int i = 0; i < 16; i++) {
      stock[i] = 10;  // Default refill to 10 units
    }
    Serial.println("✅ All boxes refilled.");
  }

  void updateStock(int box_id, int remaining) {
    if (box_id >= 1 && box_id <= 16) {
      stock[box_id - 1] = remaining;
    }
  }

  int getStock(int box_id) {
    if (box_id >= 1 && box_id <= 16) {
      return stock[box_id - 1];
    }
    return 0;
  }

  void handleScheduleActions(const std::vector<MedicineAction>& actions) {
    Serial.println("📋 Executing schedule...");
    for (const auto& action : actions) {
      if (getStock(action.box_id) >= action.dispense_amount) {
        triggerDispense(action.box_id, action.dispense_amount);
        startBuzzer(1000); // Alert after each box dispenses
      } else {
        Serial.printf("❌ Box %d: Not enough stock\n", action.box_id);
      }
    }
  }

}
