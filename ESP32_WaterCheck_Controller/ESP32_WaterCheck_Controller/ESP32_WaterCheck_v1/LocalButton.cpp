#include "LocalButton.h"
#include "pinmap.h"
#include "config.h"

static uint32_t unlockPressStartMs = 0;
static bool unlockHandled = false;

static uint32_t factoryPressStartMs = 0;
static bool factoryHandled = false;

#define BUTTON_ACTIVE_LOW true

static bool activeLowRead(uint8_t pin) {
#if BUTTON_ACTIVE_LOW
  return digitalRead(pin) == LOW;
#else
  return digitalRead(pin) == HIGH;
#endif
}

void localButtonBegin() {
  pinMode(LOCAL_RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ESTOP_BUTTON_PIN, INPUT_PULLUP);
}

bool estopIsPressed() {
  return activeLowRead(ESTOP_BUTTON_PIN);
}

bool localUnlockPressed() {
  bool pressed = activeLowRead(LOCAL_RESET_BUTTON_PIN);
  uint32_t now = millis();

  if (!pressed) {
    unlockPressStartMs = 0;
    unlockHandled = false;
    return false;
  }

  if (unlockPressStartMs == 0) unlockPressStartMs = now;

  if (!unlockHandled && (now - unlockPressStartMs >= LOCAL_UNLOCK_HOLD_MS) && (now - unlockPressStartMs < FACTORY_RESET_HOLD_MS)) {
    unlockHandled = true;
    return true;
  }

  return false;
}

bool factoryResetPressed() {
  bool pressed = activeLowRead(LOCAL_RESET_BUTTON_PIN);
  uint32_t now = millis();

  if (!pressed) {
    factoryPressStartMs = 0;
    factoryHandled = false;
    return false;
  }

  if (factoryPressStartMs == 0) factoryPressStartMs = now;

  if (!factoryHandled && (now - factoryPressStartMs >= FACTORY_RESET_HOLD_MS)) {
    factoryHandled = true;
    return true;
  }

  return false;
}
