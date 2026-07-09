#include "LocalButton.h"
#include "pinmap.h"

static bool lastStable = false;
static bool lastRaw = false;
static uint32_t lastChangeMs = 0;
static uint32_t pressStartMs = 0;
static bool pressHandled = false;

#define BUTTON_ACTIVE_LOW true
#define BUTTON_DEBOUNCE_MS 80
#define LONG_PRESS_MS 1500

void localButtonBegin() {
  pinMode(LOCAL_RESET_BUTTON_PIN, INPUT_PULLUP);
}

static bool readRaw() {
#if BUTTON_ACTIVE_LOW
  return digitalRead(LOCAL_RESET_BUTTON_PIN) == LOW;
#else
  return digitalRead(LOCAL_RESET_BUTTON_PIN) == HIGH;
#endif
}

// คืนค่า true เมื่อกดค้างเกิน LONG_PRESS_MS หนึ่งครั้ง
bool localResetPressed() {
  bool raw = readRaw();
  uint32_t now = millis();

  if (raw != lastRaw) {
    lastRaw = raw;
    lastChangeMs = now;
  }

  if ((now - lastChangeMs) >= BUTTON_DEBOUNCE_MS && lastStable != raw) {
    lastStable = raw;
    if (lastStable) {
      pressStartMs = now;
      pressHandled = false;
    } else {
      pressHandled = false;
    }
  }

  if (lastStable && !pressHandled && (now - pressStartMs >= LONG_PRESS_MS)) {
    pressHandled = true;
    return true;
  }

  return false;
}
