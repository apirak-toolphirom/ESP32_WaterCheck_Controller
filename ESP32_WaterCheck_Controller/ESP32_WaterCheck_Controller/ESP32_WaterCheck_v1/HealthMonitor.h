#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <Arduino.h>

void healthBegin();
void healthUpdate();

uint32_t healthUptimeSec();
uint32_t healthFreeHeap();
uint32_t healthMinFreeHeap();
int healthWifiRssi();
float healthChipTempC();
String healthJson();

#endif
