#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <Arduino.h>

void eventLogBegin();
void eventLogAdd(const String& type, const String& message);
String eventLogReadCsv();
String eventLogReadJson(uint16_t limit = 100);
void eventLogClear();
size_t eventLogSize();

#endif
