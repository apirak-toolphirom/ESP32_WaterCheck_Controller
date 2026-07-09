#include "MQTTClient.h"
#include "global.h"
#include "Storage.h"
#include "Relay.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

static WiFiClient wifiClient;
static PubSubClient client(wifiClient);
static uint32_t lastReconnectAttempt = 0;
static uint32_t lastPublishMs = 0;

static String topic(const String& suffix) {
  return mqttTopic + "/" + suffix;
}

static void mqttCallback(char* topicRaw, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  if (msg.startsWith("DELAY:")) storageSaveDelay(msg.substring(6).toInt());
  else if (msg.startsWith("TEST:")) storageSavePumpTest(msg.substring(5).toInt());
  else if (msg.startsWith("RETRY:")) storageSaveMaxRetry((uint8_t)msg.substring(6).toInt());
  else if (msg.startsWith("RELAYFAIL:")) storageSaveRelayFailTimeout(msg.substring(10).toInt());
  else if (msg == "PUMP_ON") manualMode = MODE_MANUAL_ON;
  else if (msg == "PUMP_OFF") manualMode = MODE_MANUAL_OFF;
  else if (msg == "AUTO") manualMode = MODE_AUTO;
  else if (msg == "MAINT_ON") maintenanceMode = true;
  else if (msg == "MAINT_OFF") maintenanceMode = false;
  else if (msg == "UNLOCK") {
    alarmActive = false;
    relayFailAlarm = false;
    currentRetryCount = 0;
    pumpState = PUMP_DELAY_WAIT;
    stateStartMs = millis();
  }
  else if (msg == "RESTART") ESP.restart();
}

void mqttBegin() {
  if (!mqttServer.length()) return;
  client.setServer(mqttServer.c_str(), mqttPort);
  client.setCallback(mqttCallback);
}

static bool mqttReconnect() {
  if (!mqttServer.length()) return false;
  String clientId = "WaterCheck-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool ok;
  if (mqttUser.length()) {
    ok = client.connect(clientId.c_str(), mqttUser.c_str(), mqttPassword.c_str(), topic("status").c_str(), 1, true, "offline");
  } else {
    ok = client.connect(clientId.c_str(), topic("status").c_str(), 1, true, "offline");
  }

  mqttConnected = ok;
  if (ok) {
    client.publish(topic("status").c_str(), "online", true);
    client.subscribe(topic("cmd").c_str());
  }
  return ok;
}

void mqttLoop() {
  if (!mqttServer.length()) return;
  if (!client.connected()) {
    mqttConnected = false;
    uint32_t now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      mqttReconnect();
    }
    return;
  }

  mqttConnected = true;
  client.loop();

  if (millis() - lastPublishMs > 5000) {
    lastPublishMs = millis();
    mqttPublishStatus();
  }
}

void mqttPublishStatus() {
  if (!client.connected()) return;
  StaticJsonDocument<512> doc;
  doc["flow"] = flowState;
  doc["pump"] = pumpRelayState;
  doc["alarm"] = alarmActive;
  doc["estop"] = estopActive;
  doc["maintenance"] = maintenanceMode;
  doc["relayFail"] = relayFailAlarm;
  doc["manualMode"] = (int)manualMode;
  doc["state"] = (int)pumpState;
  doc["retry"] = currentRetryCount;
  doc["maxRetry"] = maxRetryCount;
  doc["delay"] = delayTimeSec;
  doc["test"] = pumpTestTimeSec;
  doc["relayFailSec"] = relayFailTimeoutSec;
  doc["remain"] = remainTimeSec;
  doc["event"] = lastEvent;
  char out[512];
  size_t n = serializeJson(doc, out);
  client.publish(topic("json").c_str(), out, n);
}

void mqttPublishAlarm(const String& message) {
  if (!client.connected()) return;
  client.publish(topic("alarm").c_str(), message.c_str(), true);
}
