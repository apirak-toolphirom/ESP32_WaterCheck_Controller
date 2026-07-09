#include "GraphHistory.h"
#include "global.h"
#include "HealthMonitor.h"
#include <ArduinoJson.h>

#define GRAPH_POINTS 60

struct GraphPoint {
  uint32_t t;
  uint8_t pump;
  uint8_t flow;
  uint8_t alarm;
  uint32_t heap;
};

static GraphPoint points[GRAPH_POINTS];
static uint8_t indexWrite = 0;
static bool filled = false;
static uint32_t lastGraphMs = 0;

void graphHistoryBegin() {
  for (int i = 0; i < GRAPH_POINTS; i++) {
    points[i] = {0, 0, 0, 0, 0};
  }
}

void graphHistoryUpdate() {
  uint32_t now = millis();
  if (now - lastGraphMs < 5000) return;
  lastGraphMs = now;

  points[indexWrite].t = now / 1000UL;
  points[indexWrite].pump = pumpRelayState ? 1 : 0;
  points[indexWrite].flow = flowState ? 1 : 0;
  points[indexWrite].alarm = alarmActive ? 1 : 0;
  points[indexWrite].heap = healthFreeHeap();

  indexWrite++;
  if (indexWrite >= GRAPH_POINTS) {
    indexWrite = 0;
    filled = true;
  }
}

String graphHistoryJson() {
  StaticJsonDocument<4096> doc;
  JsonArray arr = doc.to<JsonArray>();

  uint8_t count = filled ? GRAPH_POINTS : indexWrite;
  uint8_t start = filled ? indexWrite : 0;

  for (uint8_t i = 0; i < count; i++) {
    uint8_t idx = (start + i) % GRAPH_POINTS;
    JsonObject o = arr.createNestedObject();
    o["t"] = points[idx].t;
    o["pump"] = points[idx].pump;
    o["flow"] = points[idx].flow;
    o["alarm"] = points[idx].alarm;
    o["heap"] = points[idx].heap;
  }

  String out;
  serializeJson(arr, out);
  return out;
}
