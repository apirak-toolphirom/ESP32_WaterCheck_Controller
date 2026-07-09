#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "global.h"
#include "config.h"
#include "Relay.h"
#include "Storage.h"
#include "MQTTClient.h"

static WiFiClient espClient;
static PubSubClient mqtt(espClient);
static uint32_t lastReconnect = 0;
static uint32_t lastPublish = 0;

static String topic(const String& suffix) { return settings.mqttBaseTopic + "/" + suffix; }

static void callback(char* topicIn, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i=0; i<length; i++) msg += (char)payload[i];
  msg.trim();
  if (msg == "PUMP_ON") setPump(true);
  else if (msg == "PUMP_OFF") setPump(false);
  else if (msg == "AUX_ON") setAux(true);
  else if (msg == "AUX_OFF") setAux(false);
  else if (msg.startsWith("DELAY:")) saveDelay(msg.substring(6).toInt());
  else if (msg.startsWith("TEST:")) savePumpTest(msg.substring(5).toInt());
  else if (msg == "RESTART") ESP.restart();
}

void mqttBegin() {
  if (!settings.mqttServer.length()) return;
  mqtt.setServer(settings.mqttServer.c_str(), settings.mqttPort);
  mqtt.setCallback(callback);
}

static void reconnect() {
  if (!settings.mqttServer.length() || WiFi.status() != WL_CONNECTED) return;
  if (mqtt.connected()) return;
  if (millis() - lastReconnect < MQTT_RECONNECT_MS) return;
  lastReconnect = millis();
  String clientId = String(MQTT_CLIENT_PREFIX) + String((uint32_t)ESP.getEfuseMac(), HEX);
  bool ok = settings.mqttUser.length() ? mqtt.connect(clientId.c_str(), settings.mqttUser.c_str(), settings.mqttPass.c_str(), topic("status").c_str(), 0, true, "offline") : mqtt.connect(clientId.c_str(), topic("status").c_str(), 0, true, "offline");
  if (ok) {
    mqtt.publish(topic("status").c_str(), "online", true);
    mqtt.subscribe(topic("cmd").c_str());
  }
}

void mqttPublishNow() {
  if (!mqtt.connected()) return;
  StaticJsonDocument<256> doc;
  doc["flow"] = state.flowOn;
  doc["pump"] = state.pumpOn;
  doc["aux"] = state.auxOn;
  doc["counting"] = state.counting;
  doc["testing"] = state.pumpTesting;
  doc["remain"] = state.remainSec;
  doc["delay"] = settings.delaySec;
  doc["pumpTest"] = settings.pumpTestSec;
  doc["time"] = state.dateText + " " + state.timeText;
  doc["ip"] = state.ip;
  doc["rssi"] = state.rssi;
  char buf[256];
  serializeJson(doc, buf);
  mqtt.publish(topic("telemetry").c_str(), buf, true);
}

void mqttLoop() {
  reconnect();
  state.mqttConnected = mqtt.connected();
  if (mqtt.connected()) mqtt.loop();
  if (millis() - lastPublish >= MQTT_PUBLISH_MS) {
    lastPublish = millis();
    mqttPublishNow();
  }
}
