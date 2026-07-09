#include "Relay.h"
#include "pinmap.h"
#include "global.h"

#define RELAY_ACTIVE_LOW true

static void writeRelay(uint8_t pin, bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(pin, on ? LOW : HIGH);
#else
  digitalWrite(pin, on ? HIGH : LOW);
#endif
}

void relayBegin() {
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  pinMode(RELAY_AUX_PIN, OUTPUT);
  setPump(false);
  setAuxRelay(false);
}

void setPump(bool on) {
  if (estopActive || maintenanceMode || pumpState == PUMP_LOCKOUT) {
    on = false;
  }
  if (!pumpRelayState && on) {
    pumpOnStartMs = millis();
  }
  pumpRelayState = on;
  writeRelay(RELAY_PUMP_PIN, on);
}

void setAuxRelay(bool on) {
  auxRelayState = on;
  writeRelay(RELAY_AUX_PIN, on);
}

bool getPumpRelay() { return pumpRelayState; }
bool getAuxRelay() { return auxRelayState; }
