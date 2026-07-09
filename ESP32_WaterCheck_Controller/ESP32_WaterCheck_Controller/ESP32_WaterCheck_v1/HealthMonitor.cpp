#include "HealthMonitor.h"
#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>

static uint32_t lastHealthMs = 0;
static uint32_t cachedFreeHeap = 0;
static uint32_t cachedMinFreeHeap = 0;
static int cachedRssi = 0;
static float cachedTemp = 0;

#if defined(CONFIG_IDF_TARGET_ESP32)
extern "C" uint8_t temprature_sens_read();
#endif

void healthBegin() {
  cachedFreeHeap = ESP.getFreeHeap();
  cachedMinFreeHeap = ESP.getMinFreeHeap();
  cachedRssi = WiFi.RSSI();
}

void healthUpdate() {
  uint32_t now = millis();
  if (now - lastHealthMs < HEALTH_INTERVAL_MS) return;
  lastHealthMs = now;

  cachedFreeHeap = ESP.getFreeHeap();
  cachedMinFreeHeap = ESP.getMinFreeHeap();
  cachedRssi = WiFi.isConnected() ? WiFi.RSSI() : 0;

#if defined(CONFIG_IDF_TARGET_ESP32)
  cachedTemp = (temprature_sens_read() - 32) / 1.8;
#else
  cachedTemp = 0;
#endif
}

uint32_t healthUptimeSec() {
  return millis() / 1000UL;
}

uint32_t healthFreeHeap() {
  return cachedFreeHeap;
}

uint32_t healthMinFreeHeap() {
  return cachedMinFreeHeap;
}

int healthWifiRssi() {
  return cachedRssi;
}

float healthChipTempC() {
  return cachedTemp;
}

String healthJson() {
  StaticJsonDocument<384> doc;
  doc["uptimeSec"] = healthUptimeSec();
  doc["freeHeap"] = cachedFreeHeap;
  doc["minFreeHeap"] = cachedMinFreeHeap;
  doc["wifiRssi"] = cachedRssi;
  doc["chipTempC"] = cachedTemp;
  doc["flashSize"] = ESP.getFlashChipSize();
  doc["sketchSize"] = ESP.getSketchSize();
  doc["freeSketchSpace"] = ESP.getFreeSketchSpace();
  String out;
  serializeJson(doc, out);
  return out;
}
