// ========== smart_reminder.h ==========
#ifndef SMART_REMINDER_H
#define SMART_REMINDER_H

#include <Arduino.h>

#define MAX_SCHEDULES    5
#define MAX_STOCK_PAIRS  10

//--- Schedule Entry ---
struct ScheduleEntry {
    int box_number;
    String medicine_name;
    String dosage;
    String time;
};

//--- Status Payload ---
struct StatusPayload {
    String mode;
    String result;
    String schedule_id;
    int battery_percent;
    String timestamp;
    int stock_remaining[MAX_STOCK_PAIRS][2];
    int stock_remaining_count = 0;
    String error_type;  // for error reporting
    String details;     // for error description/details

    String next_time;       // for next dose time
    String next_medicine;   // for next medicine name
};

//--- Config Payload ---
struct ConfigPayload {
    String device_time;      // ISO8601
    String timezone;
    String operation_mode;
    // Extend as needed
};

//--- Refill Payload ---
struct RefillPayload {
    int box_number;
    String medicine_name;
    int quantity;           // For request
    String result;          // "success" or "error" (for ack)
    int stock_now;          // (for ack)
};

//--- Header ---
struct SmartHeader {
    String sender;
    String device_id;
    String message_type;    // "status", "schedule", etc.
    String mode;
    String timestamp;       // ISO8601
    String version;
};

//--- Main SmartMessage ---
struct SmartMessage {
    SmartHeader header;

    struct Payload {
        ScheduleEntry schedule[MAX_SCHEDULES];  // for schedule messages
        StatusPayload status;                   // for status messages
        ConfigPayload config;                   // for config messages
        // String next_time;
        // String next_medicine;
        RefillPayload refill;                   // for refill and refill_ack
        int schedule_count = 0;
    } payload;
};

namespace SmartMedicineReminder {
    SmartMessage parseFromJson(const String& json);
    String toJson(const SmartMessage& msg);
}

#endif
