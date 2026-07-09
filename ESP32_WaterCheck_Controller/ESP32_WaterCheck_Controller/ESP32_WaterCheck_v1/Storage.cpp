#include "Storage.h"
#include "global.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;

static uint32_t clampU32(uint32_t v, uint32_t minV, uint32_t maxV) {
  if (v < minV) return minV;
  if (v > maxV) return maxV;
  return v;
}

void storageBegin() {
  prefs.begin("watercheck", false);
}

void storageLoad() {
  delayTimeSec = clampU32(prefs.getUInt("delay", DEFAULT_DELAY_SEC), MIN_DELAY_SEC, MAX_DELAY_SEC);
  pumpTestTimeSec = clampU32(prefs.getUInt("ptest", DEFAULT_PUMP_TEST_SEC), MIN_PUMP_TEST_SEC, MAX_PUMP_TEST_SEC);

  mqttServer = prefs.getString("mqttHost", "");
  mqttPort = prefs.getUShort("mqttPort", MQTT_DEFAULT_PORT);
  mqttUser = prefs.getString("mqttUser", "");
  mqttPassword = prefs.getString("mqttPass", "");
  mqttTopic = prefs.getString("mqttTopic", MQTT_TOPIC_DEFAULT);

  adminUser = prefs.getString("adminUser", ADMIN_USER_DEFAULT);
  adminPass = prefs.getString("adminPass", ADMIN_PASS_DEFAULT);
}

void storageSaveDelay(uint32_t seconds) {
  delayTimeSec = clampU32(seconds, MIN_DELAY_SEC, MAX_DELAY_SEC);
  prefs.putUInt("delay", delayTimeSec);
}

void storageSavePumpTest(uint32_t seconds) {
  pumpTestTimeSec = clampU32(seconds, MIN_PUMP_TEST_SEC, MAX_PUMP_TEST_SEC);
  prefs.putUInt("ptest", pumpTestTimeSec);
}

void storageSaveMqtt(const String& server, uint16_t port, const String& user, const String& pass, const String& topic) {
  mqttServer = server;
  mqttPort = port;
  mqttUser = user;
  mqttPassword = pass;
  mqttTopic = topic.length() ? topic : MQTT_TOPIC_DEFAULT;

  prefs.putString("mqttHost", mqttServer);
  prefs.putUShort("mqttPort", mqttPort);
  prefs.putString("mqttUser", mqttUser);
  prefs.putString("mqttPass", mqttPassword);
  prefs.putString("mqttTopic", mqttTopic);
}

void storageSaveAdmin(const String& user, const String& pass) {
  adminUser = user.length() ? user : ADMIN_USER_DEFAULT;
  adminPass = pass.length() ? pass : ADMIN_PASS_DEFAULT;
  prefs.putString("adminUser", adminUser);
  prefs.putString("adminPass", adminPass);
}
