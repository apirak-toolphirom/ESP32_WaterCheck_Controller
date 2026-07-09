#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>

void notifyBegin();
void notifyAlarm(const String& type, const String& message);
void notifyEvent(const String& type, const String& message);

#endif
