#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

enum PumpState {
  PUMP_RUNNING,
  PUMP_DELAY_WAIT,
  PUMP_TEST_RUNNING,
  PUMP_LOCKOUT,
  PUMP_ESTOP,
  PUMP_MAINTENANCE
};

enum ManualMode {
  MODE_AUTO,
  MODE_MANUAL_ON,
  MODE_MANUAL_OFF
};

extern bool flowState;
extern bool pumpRelayState;
extern bool auxRelayState;
extern bool alarmActive;
extern bool mqttConnected;
extern bool estopActive;
extern bool maintenanceMode;
extern bool relayFailAlarm;

extern PumpState pumpState;
extern ManualMode manualMode;

extern uint32_t delayTimeSec;
extern uint32_t pumpTestTimeSec;
extern uint8_t maxRetryCount;
extern uint8_t currentRetryCount;
extern uint32_t relayFailTimeoutSec;
extern uint32_t remainTimeSec;

extern uint32_t stateStartMs;
extern uint32_t lastSecondTickMs;
extern uint32_t pumpOnStartMs;

extern String lastEvent;
extern String lastFlowStopTime;
extern String lastPumpTestTime;
extern String lastFlowReturnTime;
extern String lastAlarmTime;
extern String lastRestartReason;

extern String mqttServer;
extern uint16_t mqttPort;
extern String mqttUser;
extern String mqttPassword;
extern String mqttTopic;

extern String adminUser;
extern String adminPass;

#endif
