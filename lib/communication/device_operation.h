#ifndef DEVICE_OPERATIONS_H
#define DEVICE_OPERATIONS_H

#include <Arduino.h>
#include <vector>

// Represents a single medicine dispensing action
struct MedicineAction {
  int box_id;            // Which medicine box
  int dispense_amount;   // How many pills to dispense
};

namespace DeviceOperations {

  void initHardware();                                      // Initializes GPIOs and state
  void triggerDispense(int box_id, int count);              // Dispense 'count' pills from 'box_id'
  void startBuzzer(int duration_ms);                        // Activate buzzer for duration
  void stopBuzzer();                                        // Turn off buzzer
  void enterRefillMode();                                   // Refill all boxes to default stock
  void updateStock(int box_id, int remaining);              // Manually set stock
  int getStock(int box_id);                                 // Get current stock
  void handleScheduleActions(const std::vector<MedicineAction>& actions);  // Execute multiple actions
  void startBuzzerAsync(int duration_ms);
  void updateBuzzer();  // Call this from loop() or FSM
  
}

#endif
