#include "Notification.h"
#include "MQTTClient.h"
#include "EventLog.h"

void notifyBegin() {
  eventLogAdd("SYSTEM", "Notification manager started");
}

void notifyAlarm(const String& type, const String& message) {
  eventLogAdd(type, message);
  mqttPublishAlarm(type + ": " + message);
}

void notifyEvent(const String& type, const String& message) {
  eventLogAdd(type, message);
}
