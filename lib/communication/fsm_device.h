#ifndef FSM_DEVICE_H
#define FSM_DEVICE_H

#include "smart_reminder.h"

enum DeviceState {
  IDLE,
  SCHEDULE_TRIGGERED,
  DISPENSING,
  BUZZING,
  REFILL_MODE,
  ERROR
};

namespace FSMDevice {
  void init();
  void update();                                // Call in loop()
  void handleUserAcknowledge();                 // Triggered on GPIO button press
  void processIncomingInstruction(const SmartMessage& msg); // Called from MQTT callback
  DeviceState getCurrentState();
}

#endif
// FSM_DEVICE_H