#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "config.h"
#include "pinmap.h"
#include "global.h"
#include "Storage.h"
#include "Relay.h"
#include "FlowSwitch.h"
#include "LocalButton.h"
#include "TimeSync.h"
#include "LCDDisplay.h"
#include "MQTTClient.h"
#include "Dashboard.h"

static void logEvent(const String& eventText) {
  lastEvent = eventText;
  Serial.println("[EVENT] " + eventText);
}

static void enterState(PumpState nextState) {
  pumpState = nextState;
  stateStartMs = millis();

  switch (pumpState) {
    case PUMP_RUNNING:
      setPump(true);
      remainTimeSec = 0;
      if (currentRetryCount > 0) {
        lastFlowReturnTime = dateTimeNowString();
      }
      currentRetryCount = 0;
      alarmActive = false;
      logEvent("Pump running");
      break;

    case PUMP_DELAY_WAIT:
      setPump(false);
      remainTimeSec = delayTimeSec;
      logEvent("Delay wait");
      break;

    case PUMP_TEST_RUNNING:
      currentRetryCount++;
      setPump(true);
      remainTimeSec = pumpTestTimeSec;
      lastPumpTestTime = dateTimeNowString();
      logEvent("Pump test retry " + String(currentRetryCount) + "/" + String(maxRetryCount));
      break;

    case PUMP_LOCKOUT:
      setPump(false);
      remainTimeSec = 0;
      alarmActive = true;
      lastAlarmTime = dateTimeNowString();
      logEvent("LOCKOUT: max retry reached");
      mqttPublishAlarm("LOCKOUT: max retry reached");
      break;
  }
}

static uint32_t elapsedSeconds() {
  return (millis() - stateStartMs) / 1000UL;
}

static void updateCountdown() {
  uint32_t now = millis();
  if (now - lastSecondTickMs < 1000UL) return;
  lastSecondTickMs = now;

  uint32_t target = 0;
  if (pumpState == PUMP_DELAY_WAIT) target = delayTimeSec;
  if (pumpState == PUMP_TEST_RUNNING) target = pumpTestTimeSec;

  if (target == 0) {
    remainTimeSec = 0;
    return;
  }

  uint32_t elapsed = elapsedSeconds();
  remainTimeSec = (elapsed >= target) ? 0 : (target - elapsed);
}

static void unlockFromLocalButton() {
  if (localResetPressed()) {
    alarmActive = false;
    currentRetryCount = 0;
    logEvent("Unlock from local button");
    enterState(PUMP_DELAY_WAIT);
  }
}

static void updatePumpLogic() {
  bool prevFlow = flowState;
  flowUpdate();

  if (prevFlow && !flowState) {
    lastFlowStopTime = dateTimeNowString();
    logEvent("Flow stopped");
  }

  switch (pumpState) {
    case PUMP_RUNNING:
      if (!flowIsOn()) {
        enterState(PUMP_DELAY_WAIT);
      }
      break;

    case PUMP_DELAY_WAIT:
      if (flowIsOn()) {
        enterState(PUMP_RUNNING);
        break;
      }

      if (elapsedSeconds() >= delayTimeSec) {
        if (currentRetryCount >= maxRetryCount) {
          enterState(PUMP_LOCKOUT);
        } else {
          enterState(PUMP_TEST_RUNNING);
        }
      }
      break;

    case PUMP_TEST_RUNNING:
      if (flowIsOn()) {
        enterState(PUMP_RUNNING);
        break;
      }

      if (elapsedSeconds() >= pumpTestTimeSec) {
        if (currentRetryCount >= maxRetryCount) {
          enterState(PUMP_LOCKOUT);
        } else {
          enterState(PUMP_DELAY_WAIT);
        }
      }
      break;

    case PUMP_LOCKOUT:
      setPump(false);
      unlockFromLocalButton();
      break;
  }
}

static void setupWiFi() {
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);
  bool ok = wm.autoConnect("WaterCheck_Setup");
  if (!ok) {
    Serial.println("WiFi setup failed, restarting...");
    delay(1000);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  storageBegin();
  storageLoad();

  relayBegin();
  flowBegin();
  localButtonBegin();

  setupWiFi();
  timeSyncBegin();

  lcdBegin();
  mqttBegin();
  dashboardBegin();

  enterState(PUMP_RUNNING);

  Serial.println();
  Serial.println("ESP32 Water Check Controller");
  Serial.print("Firmware: ");
  Serial.println(FIRMWARE_VERSION);
}

void loop() {
  updatePumpLogic();
  updateCountdown();

  lcdUpdate();
  mqttLoop();

  digitalWrite(STATUS_LED_PIN, alarmActive ? (millis() / 300) % 2 : (pumpRelayState ? HIGH : LOW));

  delay(2);
}
