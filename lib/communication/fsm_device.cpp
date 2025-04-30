#include "fsm_device.h"
#include "device_operation.h"
#include "smart_reminder.h"
#include "pinmap.h"

static DeviceState currentState = IDLE;
static unsigned long stateTimestamp = 0;
static SmartMessage cachedMsg;

namespace FSMDevice {

  const char* fsm_state_to_string(DeviceState state) {
    switch (state) {
      case IDLE: return "IDLE";
      case SCHEDULE_TRIGGERED: return "SCHEDULE_TRIGGERED";
      case DISPENSING: return "DISPENSING";
      case BUZZING: return "BUZZING";
      case REFILL_MODE: return "REFILL_MODE";
      case ERROR: return "ERROR";
      default: return "UNKNOWN_STATE";
    }
  }
  
  

  void transitionTo(DeviceState newState) {
    currentState = newState;
    stateTimestamp = millis();
    Serial.print("🔁 FSM Transitioned to: ");
    Serial.println(newState);
  }

  void init() {
    DeviceOperations::initHardware();
    transitionTo(IDLE);
  }

  void update() {
    Serial.print("FSM State: ");
Serial.println(fsm_state_to_string(currentState));

    switch (currentState) {
      case IDLE:
        // Waiting for command
        break;

      case SCHEDULE_TRIGGERED:
        transitionTo(DISPENSING);
        break;

      case DISPENSING:
        if (cachedMsg.payload.schedule_count > 0)
       {
          std::vector<MedicineAction> actions;
          for (auto& entry : cachedMsg.payload.schedule) {
            // Example mapping: all medicines go to box 1 for now
            actions.push_back({1, 1});
          }
          DeviceOperations::handleScheduleActions(actions);
        }
        transitionTo(BUZZING);
        break;

      case BUZZING:
        DeviceOperations::startBuzzer(2000);  // Buzz for 2s, wait for button
        break;

      case REFILL_MODE:
        DeviceOperations::enterRefillMode();
        transitionTo(IDLE);
        break;

      case ERROR:
        Serial.println("⚠️ Device in error state!");
        break;
    }
  }

  void processIncomingInstruction(const SmartMessage& msg) {
    cachedMsg = msg;

    if (msg.header.message_type == "schedule" && msg.header.mode == "operation") {
      transitionTo(SCHEDULE_TRIGGERED);
    } else if (msg.header.message_type == "refill") {
      transitionTo(REFILL_MODE);
    } else if (msg.header.message_type == "status" && msg.payload.status.result == "failure") {
      transitionTo(ERROR);
    } else {
      Serial.println("ℹ️ Unrecognized instruction type or mode.");
    }
  }

  void handleUserAcknowledge() {
    if (currentState == BUZZING) {
      DeviceOperations::stopBuzzer();
      transitionTo(IDLE);
    }
  }

  DeviceState getCurrentState() {
    return currentState;
  }

}
