#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>

void mqttBegin();
void mqttLoop();
void mqttPublishStatus();
void mqttPublishAlarm(const String& message);

#endif
