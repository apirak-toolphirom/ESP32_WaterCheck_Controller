#include "Storage.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;

void storageBegin() { prefs.begin("watercheck", false); }

void loadSettings() {
  settings.delaySec = prefs.getUInt("delay", DEFAULT_DELAY_SEC);
  if (settings.delaySec < MIN_DELAY_SEC || settings.delaySec > MAX_DELAY_SEC) settings.delaySec = DEFAULT_DELAY_SEC;

  settings.pumpTestSec = prefs.getUInt("pumpTest", DEFAULT_PUMP_TEST_SEC);
  if (settings.pumpTestSec < MIN_PUMP_TEST_SEC || settings.pumpTestSec > MAX_PUMP_TEST_SEC) settings.pumpTestSec = DEFAULT_PUMP_TEST_SEC;
  settings.mqttServer = prefs.getString("mqttSrv", "");
  settings.mqttPort = prefs.getUShort("mqttPort", MQTT_DEFAULT_PORT);
  settings.mqttUser = prefs.getString("mqttUser", "");
  settings.mqttPass = prefs.getString("mqttPass", "");
  settings.mqttBaseTopic = prefs.getString("mqttTopic", MQTT_BASE_TOPIC_DEFAULT);
  settings.adminUser = prefs.getString("admUser", ADMIN_USER_DEFAULT);
  settings.adminPass = prefs.getString("admPass", ADMIN_PASS_DEFAULT);
}

void saveDelay(uint32_t delaySec) {
  if (delaySec < MIN_DELAY_SEC) delaySec = MIN_DELAY_SEC;
  if (delaySec > MAX_DELAY_SEC) delaySec = MAX_DELAY_SEC;
  settings.delaySec = delaySec;
  prefs.putUInt("delay", delaySec);
}

void savePumpTest(uint32_t pumpTestSec) {
  if (pumpTestSec < MIN_PUMP_TEST_SEC) pumpTestSec = MIN_PUMP_TEST_SEC;
  if (pumpTestSec > MAX_PUMP_TEST_SEC) pumpTestSec = MAX_PUMP_TEST_SEC;
  settings.pumpTestSec = pumpTestSec;
  prefs.putUInt("pumpTest", pumpTestSec);
}

void saveMqtt(const String& server, uint16_t port, const String& user, const String& pass, const String& topic) {
  settings.mqttServer = server;
  settings.mqttPort = port == 0 ? MQTT_DEFAULT_PORT : port;
  settings.mqttUser = user;
  settings.mqttPass = pass;
  settings.mqttBaseTopic = topic.length() ? topic : MQTT_BASE_TOPIC_DEFAULT;
  prefs.putString("mqttSrv", settings.mqttServer);
  prefs.putUShort("mqttPort", settings.mqttPort);
  prefs.putString("mqttUser", settings.mqttUser);
  prefs.putString("mqttPass", settings.mqttPass);
  prefs.putString("mqttTopic", settings.mqttBaseTopic);
}

void saveAdmin(const String& user, const String& pass) {
  settings.adminUser = user.length() ? user : ADMIN_USER_DEFAULT;
  settings.adminPass = pass.length() ? pass : ADMIN_PASS_DEFAULT;
  prefs.putString("admUser", settings.adminUser);
  prefs.putString("admPass", settings.adminPass);
}

void factoryResetSettings() {
  prefs.clear();
  loadSettings();
}
