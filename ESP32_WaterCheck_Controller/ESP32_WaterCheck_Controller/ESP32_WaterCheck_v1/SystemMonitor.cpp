#include "SystemMonitor.h"
#include <ArduinoJson.h>
#include <Preferences.h>

static Preferences monPrefs;
static uint32_t loopCounterValue = 0;
static uint32_t wifiDisconnectCounter = 0;
static uint32_t mqttReconnectCounter = 0;
static uint32_t restartCounter = 0;
static uint32_t loopStartUs = 0;
static uint32_t loopAvgUsValue = 0;
static bool lastWifiState = false;

void systemMonitorBegin() {
  monPrefs.begin("sysmon", false);
  restartCounter = monPrefs.getUInt("restarts", 0);
  wifiDisconnectCounter = monPrefs.getUInt("wifiDisc", 0);
  mqttReconnectCounter = monPrefs.getUInt("mqttRec", 0);
}

void systemMonitorRestartBoot() {
  restartCounter++;
  monPrefs.putUInt("restarts", restartCounter);
}

void systemMonitorLoopStart() {
  loopStartUs = micros();
}

void systemMonitorLoopEnd() {
  uint32_t dt = micros() - loopStartUs;
  loopCounterValue++;
  if (loopAvgUsValue == 0) loopAvgUsValue = dt;
  else loopAvgUsValue = (loopAvgUsValue * 15 + dt) / 16;
}

void systemMonitorWifiConnected(bool connected) {
  if (lastWifiState && !connected) {
    wifiDisconnectCounter++;
    monPrefs.putUInt("wifiDisc", wifiDisconnectCounter);
  }
  lastWifiState = connected;
}

void systemMonitorMqttReconnect() {
  mqttReconnectCounter++;
  monPrefs.putUInt("mqttRec", mqttReconnectCounter);
}

uint32_t systemLoopCounter() { return loopCounterValue; }
uint32_t systemWifiDisconnectCount() { return wifiDisconnectCounter; }
uint32_t systemMqttReconnectCount() { return mqttReconnectCounter; }
uint32_t systemRestartCount() { return restartCounter; }
uint32_t systemLoopAvgUs() { return loopAvgUsValue; }

String systemMonitorJson() {
  StaticJsonDocument<384> doc;
  doc["loopCounter"] = loopCounterValue;
  doc["loopAvgUs"] = loopAvgUsValue;
  doc["wifiDisconnects"] = wifiDisconnectCounter;
  doc["mqttReconnects"] = mqttReconnectCounter;
  doc["restarts"] = restartCounter;
  String out;
  serializeJson(doc, out);
  return out;
}
