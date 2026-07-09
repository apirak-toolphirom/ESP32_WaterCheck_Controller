#include <Arduino.h>
#include "pinmap.h"
#include "config.h"
#include "global.h"
#include "Relay.h"

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
  setAux(false);
}

void setPump(bool on) {
  state.pumpOn = on;
  writeRelay(RELAY_PUMP_PIN, on);
}

void setAux(bool on) {
  state.auxOn = on;
  writeRelay(RELAY_AUX_PIN, on);
}
