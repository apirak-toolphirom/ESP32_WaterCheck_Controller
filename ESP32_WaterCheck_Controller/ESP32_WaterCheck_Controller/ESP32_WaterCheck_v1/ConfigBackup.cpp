#include "ConfigBackup.h"
#include "global.h"
#include "Storage.h"
#include "config.h"
#include <ArduinoJson.h>

static uint32_t simpleChecksum(const String& s) {
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i = 0; i < s.length(); i++) {
    crc ^= (uint8_t)s[i];
    for (int j = 0; j < 8; j++) {
      crc = (crc >> 1) ^ (0xEDB88320 & (-(int)(crc & 1)));
    }
  }
  return ~crc;
}

String configExportJson() {
  StaticJsonDocument<896> doc;
  doc["version"] = FIRMWARE_VERSION;
  doc["format"] = "watercheck-config-v1";
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

  String payload;
  serializeJson(doc, payload);
  doc["checksum"] = simpleChecksum(payload);

  String out;
  serializeJsonPretty(doc, out);
  return out;
}

bool configImportJson(const String& json) {
  StaticJsonDocument<1200> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) return false;

  if (!doc.containsKey("format")) return false;
  if (String((const char*)doc["format"]) != "watercheck-config-v1") return false;

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
