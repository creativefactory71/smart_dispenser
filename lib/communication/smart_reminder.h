// ========== smart_reminder.h ==========
#ifndef SMART_REMINDER_H
#define SMART_REMINDER_H

#include <Arduino.h>
#define MAX_SCHEDULES 5
#define MAX_STOCK_PAIRS 10

struct ScheduleEntry {
  String medicine_name;
  String time;
};

struct StatusPayload {
  String mode;
  String result;
  String schedule_id;
  std::pair<int, int> stock_remaining[MAX_STOCK_PAIRS];
  int stock_remaining_count = 0;
  int battery;
  String timestamp;
};

struct SmartMessage {
  struct {
    String sender;
    String device_id;
    String message_type;
    String mode;
    String timestamp;
    String version;
  } header;

  struct {
    ScheduleEntry schedule[MAX_SCHEDULES];
    int schedule_count = 0;
    StatusPayload status;
  } payload;
};

namespace SmartMedicineReminder {
  SmartMessage parseFromJson(const String& json);
  String toJson(const SmartMessage& msg);
}

#endif