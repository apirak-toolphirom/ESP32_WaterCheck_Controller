#include "OTAUpdate.h"
#include "config.h"
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "esp_ota_ops.h"

void otaArduinoBegin() {
  ArduinoOTA.setHostname("ESP32-WaterCheck");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]\n", error);
  });
  ArduinoOTA.begin();
}

String otaPartitionInfoJson() {
  StaticJsonDocument<512> doc;
  const esp_partition_t* running = esp_ota_get_running_partition();
  const esp_partition_t* boot = esp_ota_get_boot_partition();
  const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);

  doc["version"] = FIRMWARE_VERSION;
  doc["running"] = running ? running->label : "-";
  doc["boot"] = boot ? boot->label : "-";
  doc["next"] = next ? next->label : "-";
  doc["sketchSize"] = ESP.getSketchSize();
  doc["freeSketchSpace"] = ESP.getFreeSketchSpace();

  String out;
  serializeJson(doc, out);
  return out;
}
