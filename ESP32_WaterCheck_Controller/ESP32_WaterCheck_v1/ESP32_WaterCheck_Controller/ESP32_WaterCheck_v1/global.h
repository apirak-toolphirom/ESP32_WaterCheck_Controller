#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

struct AppSettings {
  uint32_t delaySec;
  uint32_t pumpTestSec;
  String mqttServer;
  uint16_t mqttPort;
  String mqttUser;
  String mqttPass;
  String mqttBaseTopic;
  String adminUser;
  String adminPass;
};

struct AppState {
  bool flowOn;
  bool pumpOn;
  bool auxOn;
  bool counting;
  bool pumpTesting;
  uint32_t remainSec;
  uint32_t totalDelaySec;
  String timeText;
  String dateText;
  String ip;
  int rssi;
  bool mqttConnected;
  String lastEvent;
};

extern AppSettings settings;
extern AppState state;

#endif
