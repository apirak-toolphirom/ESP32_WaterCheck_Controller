#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <esp_task_wdt.h>

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
#include "HealthMonitor.h"
#include "OTAUpdate.h"
#include "EventLog.h"
#include "Notification.h"

static void logEvent(const String& eventText) {
  lastEvent = eventText;
  Serial.println("[EVENT] " + eventText);
  eventLogAdd("EVENT", eventText);
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
        notifyEvent("FLOW_RETURN", "Flow returned");
      }
      currentRetryCount = 0;
      alarmActive = false;
      relayFailAlarm = false;
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
      notifyEvent("PUMP_TEST", "Retry " + String(currentRetryCount) + "/" + String(maxRetryCount));
      break;

    case PUMP_LOCKOUT:
      setPump(false);
      remainTimeSec = 0;
      alarmActive = true;
      lastAlarmTime = dateTimeNowString();
      logEvent("LOCKOUT: max retry reached");
      notifyAlarm("LOCKOUT", "max retry reached");
      break;

    case PUMP_ESTOP:
      setPump(false);
      remainTimeSec = 0;
      alarmActive = true;
      lastAlarmTime = dateTimeNowString();
      logEvent("E-STOP active");
      notifyAlarm("ESTOP", "E-STOP active");
      break;

    case PUMP_MAINTENANCE:
      setPump(false);
      remainTimeSec = 0;
      logEvent("Maintenance mode");
      notifyEvent("MAINTENANCE", "Maintenance mode");
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

static void checkRelayFail() {
  if (!pumpRelayState || flowState || relayFailAlarm) return;
  if ((millis() - pumpOnStartMs) / 1000UL >= relayFailTimeoutSec) {
    relayFailAlarm = true;
    alarmActive = true;
    lastAlarmTime = dateTimeNowString();
    logEvent("Relay fail or no-flow after pump ON");
    notifyAlarm("RELAY_FAIL", "no-flow after pump ON");
    enterState(PUMP_LOCKOUT);
  }
}

static void handleLocalButtons() {
  bool estopNow = estopIsPressed();

  if (estopNow && !estopActive) {
    estopActive = true;
    enterState(PUMP_ESTOP);
  } else if (!estopNow && estopActive) {
    estopActive = false;
    logEvent("E-STOP released");
    notifyEvent("ESTOP", "E-STOP released");
    enterState(PUMP_DELAY_WAIT);
  }

  if (factoryResetPressed()) {
    logEvent("Factory reset requested");
    notifyAlarm("FACTORY_RESET", "Factory reset requested");
    storageClearAll();
    delay(500);
    ESP.restart();
  }

  if (localUnlockPressed()) {
    alarmActive = false;
    relayFailAlarm = false;
    currentRetryCount = 0;
    maintenanceMode = false;
    manualMode = MODE_AUTO;
    logEvent("Unlock from local button");
    notifyEvent("UNLOCK", "Unlock from local button");
    enterState(PUMP_DELAY_WAIT);
  }
}

static void updateManualAndMaintenance() {
  if (estopActive) {
    if (pumpState != PUMP_ESTOP) enterState(PUMP_ESTOP);
    return;
  }

  if (maintenanceMode) {
    if (pumpState != PUMP_MAINTENANCE) enterState(PUMP_MAINTENANCE);
    setPump(false);
    return;
  }

  if (manualMode == MODE_MANUAL_ON) {
    setPump(true);
    return;
  }

  if (manualMode == MODE_MANUAL_OFF) {
    setPump(false);
    return;
  }
}

static void updatePumpLogic() {
  handleLocalButtons();

  bool prevFlow = flowState;
  flowUpdate();

  if (prevFlow && !flowState) {
    lastFlowStopTime = dateTimeNowString();
    logEvent("Flow stopped");
    notifyEvent("FLOW_STOP", "Flow stopped");
  }

  updateManualAndMaintenance();
  if (estopActive || maintenanceMode || manualMode != MODE_AUTO) return;

  switch (pumpState) {
    case PUMP_RUNNING:
      if (!flowIsOn()) {
        lastFlowStopTime = dateTimeNowString();
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
      break;

    case PUMP_ESTOP:
    case PUMP_MAINTENANCE:
      setPump(false);
      break;
  }

  checkRelayFail();
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

static void setupWatchdog() {
#if ESP_IDF_VERSION_MAJOR >= 5
  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = 10000,
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
    .trigger_panic = true,
  };
  esp_task_wdt_init(&twdt_config);
#else
  esp_task_wdt_init(10, true);
#endif
  esp_task_wdt_add(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  lastRestartReason = String((int)esp_reset_reason());

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  storageBegin();
  storageLoad();

  relayBegin();
  flowBegin();
  localButtonBegin();

  setupWiFi();
  timeSyncBegin();

  eventLogBegin();
  notifyBegin();

  lcdBegin();
  mqttBegin();
  dashboardBegin();
  healthBegin();
  otaArduinoBegin();
  setupWatchdog();

  eventLogAdd("SYSTEM", "Boot firmware " + String(FIRMWARE_VERSION));
  eventLogAdd("SYSTEM", "Reset reason " + lastRestartReason);

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
  dashboardLoop();
  healthUpdate();
  ArduinoOTA.handle();

  digitalWrite(STATUS_LED_PIN, alarmActive ? (millis() / 300) % 2 : (pumpRelayState ? HIGH : LOW));

  esp_task_wdt_reset();
  delay(2);
}
