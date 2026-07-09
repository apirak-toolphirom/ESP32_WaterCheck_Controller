#include "ConfigBackup.h"
#include "global.h"
#include "Storage.h"
#include "config.h"
#include <ArduinoJson.h>

String configExportJson() {
  StaticJsonDocument<768> doc;
  doc["version"] = FIRMWARE_VERSION;
  doc["delay"] = delayTimeSec;
  doc["pumpTest"] = pumpTestTimeSec;
  doc["maxRetry"] = maxRetryCount;
  doc["relayFailSec"] = relayFailTimeoutSec;
  doc["mqttServer"] = mqttServer;
  doc["mqttPort"] = mqttPort;
  doc["mqttUser"] = mqttUser;
  doc["mqttPassword"] = mqttPassword;
  doc["mqttTopic"] = mqttTopic;
  doc["adminUser"] = adminUser;
  doc["adminPass"] = adminPass;

  String out;
  serializeJsonPretty(doc, out);
  return out;
}

bool configImportJson(const String& json) {
  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) return false;

  if (doc.containsKey("delay")) storageSaveDelay(doc["delay"].as<uint32_t>());
  if (doc.containsKey("pumpTest")) storageSavePumpTest(doc["pumpTest"].as<uint32_t>());
  if (doc.containsKey("maxRetry")) storageSaveMaxRetry(doc["maxRetry"].as<uint8_t>());
  if (doc.containsKey("relayFailSec")) storageSaveRelayFailTimeout(doc["relayFailSec"].as<uint32_t>());

  if (doc.containsKey("mqttServer")) {
    storageSaveMqtt(
      doc["mqttServer"].as<String>(),
      doc["mqttPort"] | MQTT_DEFAULT_PORT,
      doc["mqttUser"] | "",
      doc["mqttPassword"] | "",
      doc["mqttTopic"] | MQTT_TOPIC_DEFAULT
    );
  }

  if (doc.containsKey("adminUser") || doc.containsKey("adminPass")) {
    storageSaveAdmin(
      doc["adminUser"] | ADMIN_USER_DEFAULT,
      doc["adminPass"] | ADMIN_PASS_DEFAULT
    );
  }

  return true;
}
