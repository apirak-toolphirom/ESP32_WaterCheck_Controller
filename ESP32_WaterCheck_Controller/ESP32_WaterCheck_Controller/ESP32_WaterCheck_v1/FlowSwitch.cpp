#include "FlowSwitch.h"
#include "pinmap.h"
#include "global.h"

static bool stableState = false;
static bool lastRawState = false;
static uint32_t lastChangeMs = 0;
static const uint32_t DEBOUNCE_MS = 80;

// true  = INPUT_PULLUP และ Flow Switch ดึงลง GND เมื่อ ON
// false = มีวงจรภายนอกจ่าย HIGH เมื่อ ON
#define FLOW_USE_PULLUP true

void flowBegin() {
#if FLOW_USE_PULLUP
  pinMode(FLOW_SWITCH_PIN, INPUT_PULLUP);
#else
  pinMode(FLOW_SWITCH_PIN, INPUT);
#endif
  bool raw = flowReadRaw();
  stableState = raw;
  lastRawState = raw;
  flowState = stableState;
}

bool flowReadRaw() {
#if FLOW_USE_PULLUP
  return digitalRead(FLOW_SWITCH_PIN) == LOW;
#else
  return digitalRead(FLOW_SWITCH_PIN) == HIGH;
#endif
}

bool flowUpdate() {
  bool raw = flowReadRaw();
  uint32_t now = millis();

  if (raw != lastRawState) {
    lastRawState = raw;
    lastChangeMs = now;
  }

  if ((now - lastChangeMs) >= DEBOUNCE_MS && stableState != raw) {
    stableState = raw;
    flowState = stableState;
  }

  return flowState;
}

bool flowIsOn() {
  return flowState;
}
