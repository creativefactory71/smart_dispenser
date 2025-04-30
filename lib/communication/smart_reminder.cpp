// ========== smart_reminder.cpp ==========
#include "smart_reminder.h"
#include <ArduinoJson.h>

SmartMessage SmartMedicineReminder::parseFromJson(const String& json) {
  SmartMessage msg;
  JsonDocument doc;
  deserializeJson(doc, json);

  JsonObject header = doc["header"];
  msg.header.sender = header["sender"].as<String>();
  msg.header.device_id = header["device_id"].as<String>();
  msg.header.message_type = header["message_type"].as<String>();
  msg.header.mode = header["mode"].as<String>();
  msg.header.timestamp = header["timestamp"].as<String>();
  msg.header.version = header["version"].as<String>();

  JsonArray scheduleArray = doc["payload"]["schedule"];
  int i = 0;
  for (JsonObject item : scheduleArray) {
    if (i >= MAX_SCHEDULES) break;
    msg.payload.schedule[i].medicine_name = item["medicine_name"].as<String>();
    msg.payload.schedule[i].time = item["time"].as<String>();
    i++;
  }
  msg.payload.schedule_count = i;

  JsonObject status = doc["payload"]["status"];
  msg.payload.status.mode = status["mode"].as<String>();
  msg.payload.status.result = status["result"].as<String>();
  msg.payload.status.schedule_id = status["schedule_id"].as<String>();
  msg.payload.status.battery = status["battery"];
  msg.payload.status.timestamp = status["timestamp"].as<String>();

  JsonArray stockArray = status["stock_remaining"];
  int j = 0;
  for (JsonArray pair : stockArray) {
    if (j >= MAX_STOCK_PAIRS) break;
    msg.payload.status.stock_remaining[j] = { pair[0], pair[1] };
    j++;
  }
  msg.payload.status.stock_remaining_count = j;

  return msg;
}

String SmartMedicineReminder::toJson(const SmartMessage& msg) {
  JsonDocument doc;
  JsonObject header = doc["header"].to<JsonObject>();
  header["sender"] = msg.header.sender;
  header["device_id"] = msg.header.device_id;
  header["message_type"] = msg.header.message_type;
  header["mode"] = msg.header.mode;
  header["timestamp"] = msg.header.timestamp;
  header["version"] = msg.header.version;

  JsonArray scheduleArray = doc["payload"]["schedule"].to<JsonArray>();
  for (int i = 0; i < msg.payload.schedule_count; i++) {
    JsonObject item = scheduleArray.add<JsonObject>();
    item["medicine_name"] = msg.payload.schedule[i].medicine_name;
    item["time"] = msg.payload.schedule[i].time;
  }

  JsonObject status = doc["payload"]["status"].to<JsonObject>();
  status["mode"] = msg.payload.status.mode;
  status["result"] = msg.payload.status.result;
  status["schedule_id"] = msg.payload.status.schedule_id;
  status["battery"] = msg.payload.status.battery;
  status["timestamp"] = msg.payload.status.timestamp;

  JsonArray stockArray = status["stock_remaining"].to<JsonArray>();
  for (int j = 0; j < msg.payload.status.stock_remaining_count; j++) {
    JsonArray pair = stockArray.add<JsonArray>();
    pair.add(msg.payload.status.stock_remaining[j].first);
    pair.add(msg.payload.status.stock_remaining[j].second);
  }

  String output;
  serializeJson(doc, output);
  return output;
}
