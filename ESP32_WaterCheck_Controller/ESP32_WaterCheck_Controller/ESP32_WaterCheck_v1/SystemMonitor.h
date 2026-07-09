#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <Arduino.h>

void systemMonitorBegin();
void systemMonitorLoopStart();
void systemMonitorLoopEnd();
void systemMonitorWifiConnected(bool connected);
void systemMonitorMqttReconnect();
void systemMonitorRestartBoot();

uint32_t systemLoopCounter();
uint32_t systemWifiDisconnectCount();
uint32_t systemMqttReconnectCount();
uint32_t systemRestartCount();
uint32_t systemLoopAvgUs();
String systemMonitorJson();

#endif
