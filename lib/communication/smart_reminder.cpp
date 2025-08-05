// ========== smart_reminder.cpp ==========

#include "smart_reminder.h"
#include <ArduinoJson.h>

#define JSON_DOC_SIZE 2048

SmartMessage SmartMedicineReminder::parseFromJson(const String& json) {
  SmartMessage msg;
  DynamicJsonDocument doc(JSON_DOC_SIZE);
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    // Optionally log error or set error state
    return msg;
  }

  // Parse header
  JsonObject header = doc["header"];
  msg.header.sender        = header["sender"]       | "";
  msg.header.device_id     = header["device_id"]    | "";
  msg.header.message_type  = header["message_type"] | "";
  msg.header.mode          = header["mode"]         | "";
  msg.header.timestamp     = header["timestamp"]    | "";
  msg.header.version       = header["version"]      | "";

  // Parse config payload (optional)
  if (doc["payload"].containsKey("config")) {
    JsonObject config = doc["payload"]["config"];
    msg.payload.config.device_time    = config["device_time"]    | "";
    msg.payload.config.timezone       = config["timezone"]       | "";
    msg.payload.config.operation_mode = config["operation_mode"] | "";
  }

  // Parse schedule array
  if (doc["payload"].containsKey("schedule")) {
    JsonArray scheduleArray = doc["payload"]["schedule"];
    int i = 0;
    for (JsonObject item : scheduleArray) {
      if (i >= MAX_SCHEDULES) break;
      msg.payload.schedule[i].box_number    = item["box_number"]    | 0;
      msg.payload.schedule[i].medicine_name = item["medicine_name"] | "";
      msg.payload.schedule[i].dosage        = item["dosage"]        | "";
      msg.payload.schedule[i].time          = item["time"]          | "";
      i++;
    }
    msg.payload.schedule_count = i;
  }

  // Parse status payload
  if (doc["payload"].containsKey("status")) {
    JsonObject status = doc["payload"]["status"];
    msg.payload.status.mode         = status["mode"]         | "";
    msg.payload.status.result       = status["result"]       | "";
    msg.payload.status.schedule_id  = status["schedule_id"]  | "";
    msg.payload.status.battery_percent = status["battery_percent"] | 0;
    msg.payload.status.timestamp    = status["timestamp"]    | "";

    // Parse error reporting fields (if present)
    msg.payload.status.error_type   = status["error_type"]   | "";
    msg.payload.status.details      = status["details"]      | "";

    // Parse next_time and next_medicine (NEW)
    msg.payload.status.next_time     = status["next_time"]    | "";
    msg.payload.status.next_medicine = status["next_medicine"]| "";

    // Parse stock_remaining
    if (status.containsKey("stock_remaining")) {
      JsonArray stockArray = status["stock_remaining"];
      int j = 0;
      for (JsonArray pair : stockArray) {
        if (j >= MAX_STOCK_PAIRS) break;
        msg.payload.status.stock_remaining[j][0] = pair[0] | 0;
        msg.payload.status.stock_remaining[j][1] = pair[1] | 0;
        j++;
      }
      msg.payload.status.stock_remaining_count = j;
    }
  }

  // // Parse refill payload (if present)
  // if (doc["payload"].containsKey("refill")) {
  //   JsonObject refill = doc["payload"]["refill"];
  //   msg.payload.refill.box_number    = refill["box_number"]    | 0;
  //   msg.payload.refill.medicine_name = refill["medicine_name"] | "";
  //   msg.payload.refill.quantity      = refill["quantity"]      | 0;
  //   msg.payload.refill.result        = refill["result"]        | "";
  //   msg.payload.refill.stock_now     = refill["stock_now"]     | 0;
  // }

  return msg;
}

String SmartMedicineReminder::toJson(const SmartMessage& msg) {
  DynamicJsonDocument doc(JSON_DOC_SIZE);

  // Header
  JsonObject header = doc.createNestedObject("header");
  header["sender"]        = msg.header.sender;
  header["device_id"]     = msg.header.device_id;
  header["message_type"]  = msg.header.message_type;
  header["mode"]          = msg.header.mode;
  header["timestamp"]     = msg.header.timestamp;
  header["version"]       = msg.header.version;

  // Schedule array
  if (msg.payload.schedule_count > 0) {
    JsonArray scheduleArray = doc["payload"].createNestedArray("schedule");
    for (int i = 0; i < msg.payload.schedule_count; i++) {
      JsonObject item = scheduleArray.createNestedObject();
      item["box_number"]    = msg.payload.schedule[i].box_number;
      item["medicine_name"] = msg.payload.schedule[i].medicine_name;
      item["dosage"]        = msg.payload.schedule[i].dosage;
      item["time"]          = msg.payload.schedule[i].time;
    }
  }

  // Status
  if (!msg.payload.status.mode.isEmpty()) {
    JsonObject status = doc["payload"].createNestedObject("status");
    status["mode"]         = msg.payload.status.mode;
    status["result"]       = msg.payload.status.result;
    status["schedule_id"]  = msg.payload.status.schedule_id;
    status["battery_percent"] = msg.payload.status.battery_percent;
    status["timestamp"]    = msg.payload.status.timestamp;
    status["error_type"]   = msg.payload.status.error_type;
    status["details"]      = msg.payload.status.details;
    // Write next_time and next_medicine (NEW)
    status["next_time"]    = msg.payload.status.next_time;
    status["next_medicine"]= msg.payload.status.next_medicine;

    // Stock Remaining Array
    if (msg.payload.status.stock_remaining_count > 0) {
      JsonArray stockArray = status.createNestedArray("stock_remaining");
      for (int j = 0; j < msg.payload.status.stock_remaining_count; j++) {
        JsonArray pair = stockArray.createNestedArray();
        pair.add(msg.payload.status.stock_remaining[j][0]);
        pair.add(msg.payload.status.stock_remaining[j][1]);
      }
    }
  }

  // Config
  if (!msg.payload.config.device_time.isEmpty()) {
    JsonObject config = doc["payload"].createNestedObject("config");
    config["device_time"]    = msg.payload.config.device_time;
    config["timezone"]       = msg.payload.config.timezone;
    config["operation_mode"] = msg.payload.config.operation_mode;
  }

  // Refill
  // if (msg.payload.refill.box_number > 0 || !msg.payload.refill.result.isEmpty()) {
  //   JsonObject refill = doc["payload"].createNestedObject("refill");
  //   refill["box_number"]    = msg.payload.refill.box_number;
  //   refill["medicine_name"] = msg.payload.refill.medicine_name;
  //   refill["quantity"]      = msg.payload.refill.quantity;
  //   refill["result"]        = msg.payload.refill.result;
  //   refill["stock_now"]     = msg.payload.refill.stock_now;
  // }

  String output;
  serializeJson(doc, output);
  return output;
}
