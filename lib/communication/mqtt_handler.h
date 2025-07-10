#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H


extern volatile bool cmdConfirmFlag; // Set this to true whenever you want to blink
void setupMQTT();
void mqttLoop();
void publishStatusPeriodically();

#endif
