#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

enum PumpState {
  PUMP_RUNNING,
  PUMP_DELAY_WAIT,
  PUMP_TEST_RUNNING,
  PUMP_LOCKOUT
};

extern bool flowState;
extern bool pumpRelayState;
extern bool auxRelayState;
extern bool alarmActive;
extern bool mqttConnected;

extern PumpState pumpState;

extern uint32_t delayTimeSec;
extern uint32_t pumpTestTimeSec;
extern uint8_t maxRetryCount;
extern uint8_t currentRetryCount;
extern uint32_t remainTimeSec;

extern uint32_t stateStartMs;
extern uint32_t lastSecondTickMs;

extern String lastEvent;
extern String lastFlowStopTime;
extern String lastPumpTestTime;
extern String lastFlowReturnTime;
extern String lastAlarmTime;

extern String mqttServer;
extern uint16_t mqttPort;
extern String mqttUser;
extern String mqttPassword;
extern String mqttTopic;

extern String adminUser;
extern String adminPass;

#endif
