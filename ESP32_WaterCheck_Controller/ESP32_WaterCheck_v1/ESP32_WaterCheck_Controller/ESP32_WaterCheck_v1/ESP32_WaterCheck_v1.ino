#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "pinmap.h"
#include "global.h"
#include "Storage.h"
#include "Relay.h"
#include "FlowSwitch.h"
#include "LCDDisplay.h"
#include "TimeSync.h"
#include "MQTTClient.h"
#include "Dashboard.h"

AppSettings settings;
AppState state;

static uint32_t noFlowStartMs = 0;
static uint32_t pumpTestStartMs = 0;
static uint32_t lastBlinkMs = 0;
static uint32_t lastWifiOkMs = 0;

static void updateNetworkState() {
  if (WiFi.status() == WL_CONNECTED) {
    state.ip = WiFi.localIP().toString();
    state.rssi = WiFi.RSSI();
    lastWifiOkMs = millis();
  } else {
    state.ip = "0.0.0.0";
    state.rssi = 0;
    if (lastWifiOkMs > 0 && millis() - lastWifiOkMs > WIFI_RESTART_AFTER_MS) ESP.restart();
  }
}

static void updateControlLogic() {
  flowUpdate();

  // Normal condition: water flow is present.
  // Keep pump ON and clear all countdown/test states.
  if (state.flowOn) {
    state.counting = false;
    state.pumpTesting = false;
    state.remainSec = 0;
    state.totalDelaySec = 0;
    noFlowStartMs = 0;
    pumpTestStartMs = 0;
    if (!state.pumpOn) setPump(true);
    return;
  }

  // Pump test window after delay timeout.
  // The pump runs for settings.pumpTestSec seconds to check whether flow returns.
  // If flow returns, the state.flowOn branch above will clear this mode.
  if (state.pumpTesting) {
    if (!state.pumpOn) setPump(true);

    uint32_t elapsedTestSec = (millis() - pumpTestStartMs) / 1000UL;
    state.counting = false;
    state.totalDelaySec = settings.pumpTestSec;

    if (elapsedTestSec >= settings.pumpTestSec) {
      // Still no flow after test run: stop pump and start a new delay cycle.
      state.pumpTesting = false;
      state.remainSec = 0;
      pumpTestStartMs = 0;
      noFlowStartMs = millis();
      state.counting = true;
      state.totalDelaySec = settings.delaySec;
      state.lastEvent = "Pump test ended: No flow, restart delay";
      setPump(false);
    } else {
      state.remainSec = settings.pumpTestSec - elapsedTestSec;
      state.lastEvent = "Pump test running";
    }
    return;
  }

  // No flow and not in pump test: start or continue delay countdown.
  if (noFlowStartMs == 0) {
    noFlowStartMs = millis();
    state.counting = true;
    state.totalDelaySec = settings.delaySec;
    state.lastEvent = "Start delay countdown";
    setPump(false);
  }

  uint32_t elapsedSec = (millis() - noFlowStartMs) / 1000UL;
  if (elapsedSec >= settings.delaySec) {
    // Delay complete: start pump test run instead of looping ON/OFF immediately.
    state.remainSec = settings.pumpTestSec;
    state.counting = false;
    state.pumpTesting = true;
    state.totalDelaySec = settings.pumpTestSec;
    pumpTestStartMs = millis();
    noFlowStartMs = 0;
    state.lastEvent = "Delay complete: Start pump test";
    setPump(true);
  } else {
    state.remainSec = settings.delaySec - elapsedSec;
  }
}

static void blinkStatus() {
  if (millis() - lastBlinkMs >= 500) {
    lastBlinkMs = millis();
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  storageBegin();
  loadSettings();

  relayBegin();
  flowBegin();
  lcdBegin();

  WiFiManager wm;
  wm.setConfigPortalTimeout(180);
  bool ok = wm.autoConnect("WaterCheck_Setup");
  if (!ok) ESP.restart();
  lastWifiOkMs = millis();

  timeBegin();
  mqttBegin();
  dashboardBegin();

  state.lastEvent = "System started";
  setPump(true);
}

void loop() {
  updateNetworkState();
  timeUpdate();
  updateControlLogic();
  lcdUpdate();
  mqttLoop();
  dashboardLoop();
  blinkStatus();
  delay(2);
}
