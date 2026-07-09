#include <Arduino.h>
#include "pinmap.h"
#include "config.h"
#include "global.h"
#include "FlowSwitch.h"

static bool lastRaw = false;
static bool stable = false;
static uint32_t lastChange = 0;

void flowBegin() {
  pinMode(FLOW_SWITCH_PIN, INPUT);
  lastRaw = digitalRead(FLOW_SWITCH_PIN) == HIGH;
  stable = lastRaw;
  state.flowOn = stable;
}

bool flowUpdate() {
  bool raw = digitalRead(FLOW_SWITCH_PIN) == HIGH;
  uint32_t now = millis();
  if (raw != lastRaw) {
    lastRaw = raw;
    lastChange = now;
  }
  if ((now - lastChange) >= FLOW_DEBOUNCE_MS && stable != raw) {
    stable = raw;
    state.flowOn = stable;
    state.lastEvent = stable ? "Flow ON" : "Flow OFF";
    return true;
  }
  return false;
}
