// // SmartMedicineReminder.h
// #ifndef SMART_MEDICINE_REMINDER_H
// #define SMART_MEDICINE_REMINDER_H

// #include <Arduino.h>
// #include <vector>
// #include <map>
// #include <ArduinoJson.h>

// struct MessageHeader {
//   String device_id;
//   String user_id;
//   String message_type;
//   String mode;
//   String timestamp;
//   String version;
// };

// struct ScheduleEntry {
//   String schedule_id;
//   String medicine_name;
//   String dosage;
//   int box_no;
//   int quantity;
//   String time;
//   std::vector<String> repeat;
//   String start_date;
//   String end_date;
//   String reminder_type;
//   String notes;
// };

// struct RefillBox {
//   int box_no;
//   String medicine_name;
//   int initial_stock;
// };

// struct Command {
//   String action;
//   int target_box;
// };

// struct Status {
//   String mode;
//   String dispense_result;
//   String dispensed_schedule_id;
//   std::map<int, int> stock_remaining;
//   int battery;
//   String last_action_time;
// };

// struct AlertDetail {
//   int box_no;
//   String medicine_name;
//   int stock_level;
// };

// struct Alert {
//   String alert_type;
//   std::vector<AlertDetail> details;
// };

// struct Payload {
//   std::vector<ScheduleEntry> schedule;
//   std::vector<RefillBox> refill_boxes;
//   Command command;
//   Status status;
//   Alert alert;
// };

// struct SmartMessage {
//   MessageHeader header;
//   Payload payload;
// };

// class SmartMedicineReminder {
//   public:
//     static SmartMessage parseFromJson(const String& json);
//     static String toJson(const SmartMessage& msg);
//   };
  
// #endif
