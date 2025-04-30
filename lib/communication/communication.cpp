#include "communication.h"

SmartMessage SmartMedicineReminder::parseFromJson(const String& json) {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, json);
  SmartMessage msg;

  JsonObject hdr = doc["header"];
  msg.header.device_id = hdr["device_id"].as<String>();
  msg.header.user_id = hdr["user_id"].as<String>();
  msg.header.message_type = hdr["message_type"].as<String>();
  msg.header.mode = hdr["mode"].as<String>();
  msg.header.timestamp = hdr["timestamp"].as<String>();
  msg.header.version = hdr["version"].as<String>();

  JsonArray schedules = doc["payload"]["schedule"];
  for (JsonObject sched : schedules) {
    ScheduleEntry s;
    s.schedule_id = sched["schedule_id"].as<String>();
    s.medicine_name = sched["medicine_name"].as<String>();
    s.dosage = sched["dosage"].as<String>();
    s.box_no = sched["box_no"];
    s.quantity = sched["quantity"];
    s.time = sched["time"].as<String>();
    for (JsonVariant day : sched["repeat"].as<JsonArray>()) {
      s.repeat.push_back(day.as<String>());
    }
    s.start_date = sched["start_date"].as<String>();
    s.end_date = sched["end_date"].as<String>();
    s.reminder_type = sched["reminder_type"].as<String>();
    s.notes = sched["notes"].as<String>();
    msg.payload.schedule.push_back(s);
  }

  JsonArray refillBoxes = doc["payload"]["refill"]["boxes"];
  for (JsonObject box : refillBoxes) {
    RefillBox rb;
    rb.box_no = box["box_no"];
    rb.medicine_name = box["medicine_name"].as<String>();
    rb.initial_stock = box["initial_stock"];
    msg.payload.refill_boxes.push_back(rb);
  }

  JsonObject cmd = doc["payload"]["command"];
  msg.payload.command.action = cmd["action"].as<String>();
  msg.payload.command.target_box = cmd["target_box"];

  JsonObject status = doc["payload"]["status"];
  msg.payload.status.mode = status["mode"].as<String>();
  msg.payload.status.dispense_result = status["dispense_result"].as<String>();
  msg.payload.status.dispensed_schedule_id = status["dispensed_schedule_id"].as<String>();
  for (JsonPair kv : status["stock_remaining"].as<JsonObject>()) {
    msg.payload.status.stock_remaining[atoi(kv.key().c_str())] = kv.value().as<int>();
  }
  msg.payload.status.battery = status["battery"];
  msg.payload.status.last_action_time = status["last_action_time"].as<String>();

  JsonArray alertDetails = doc["payload"]["alert"]["details"];
  msg.payload.alert.alert_type = doc["payload"]["alert"]["alert_type"].as<String>();
  for (JsonObject detail : alertDetails) {
    AlertDetail a;
    a.box_no = detail["box_no"];
    a.medicine_name = detail["medicine_name"].as<String>();
    a.stock_level = detail["stock_level"];
    msg.payload.alert.details.push_back(a);
  }

  return msg;
}

String SmartMedicineReminder::toJson(const SmartMessage& msg) {
  DynamicJsonDocument doc(4096);

  JsonObject hdr = doc.createNestedObject("header");
  hdr["device_id"] = msg.header.device_id;
  hdr["user_id"] = msg.header.user_id;
  hdr["message_type"] = msg.header.message_type;
  hdr["mode"] = msg.header.mode;
  hdr["timestamp"] = msg.header.timestamp;
  hdr["version"] = msg.header.version;

  JsonArray schedules = doc["payload"].createNestedArray("schedule");
  for (const auto& s : msg.payload.schedule) {
    JsonObject sched = schedules.createNestedObject();
    sched["schedule_id"] = s.schedule_id;
    sched["medicine_name"] = s.medicine_name;
    sched["dosage"] = s.dosage;
    sched["box_no"] = s.box_no;
    sched["quantity"] = s.quantity;
    sched["time"] = s.time;
    JsonArray rep = sched.createNestedArray("repeat");
    for (const auto& r : s.repeat) rep.add(r);
    sched["start_date"] = s.start_date;
    sched["end_date"] = s.end_date;
    sched["reminder_type"] = s.reminder_type;
    sched["notes"] = s.notes;
  }

  JsonArray refillBoxes = doc["payload"].createNestedObject("refill").createNestedArray("boxes");
  for (const auto& rb : msg.payload.refill_boxes) {
    JsonObject box = refillBoxes.createNestedObject();
    box["box_no"] = rb.box_no;
    box["medicine_name"] = rb.medicine_name;
    box["initial_stock"] = rb.initial_stock;
  }

  JsonObject cmd = doc["payload"].createNestedObject("command");
  cmd["action"] = msg.payload.command.action;
  cmd["target_box"] = msg.payload.command.target_box;

  JsonObject status = doc["payload"].createNestedObject("status");
  status["mode"] = msg.payload.status.mode;
  status["dispense_result"] = msg.payload.status.dispense_result;
  status["dispensed_schedule_id"] = msg.payload.status.dispensed_schedule_id;
  JsonObject stock = status.createNestedObject("stock_remaining");
  for (const auto& kv : msg.payload.status.stock_remaining) {
    stock[String(kv.first)] = kv.second;
  }
  status["battery"] = msg.payload.status.battery;
  status["last_action_time"] = msg.payload.status.last_action_time;

  JsonObject alert = doc["payload"].createNestedObject("alert");
  alert["alert_type"] = msg.payload.alert.alert_type;
  JsonArray details = alert.createNestedArray("details");
  for (const auto& d : msg.payload.alert.details) {
    JsonObject det = details.createNestedObject();
    det["box_no"] = d.box_no;
    det["medicine_name"] = d.medicine_name;
    det["stock_level"] = d.stock_level;
  }

  String output;
  serializeJson(doc, output);
  return output;
}
