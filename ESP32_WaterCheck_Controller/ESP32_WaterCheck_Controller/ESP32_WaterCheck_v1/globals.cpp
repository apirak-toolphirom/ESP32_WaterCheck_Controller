#include "global.h"
#include "config.h"

bool flowState = false;
bool pumpRelayState = false;
bool auxRelayState = false;
bool alarmActive = false;
bool mqttConnected = false;

PumpState pumpState = PUMP_RUNNING;

uint32_t delayTimeSec = DEFAULT_DELAY_SEC;
uint32_t pumpTestTimeSec = DEFAULT_PUMP_TEST_SEC;
uint8_t maxRetryCount = DEFAULT_MAX_RETRY;
uint8_t currentRetryCount = 0;
uint32_t remainTimeSec = 0;

uint32_t stateStartMs = 0;
uint32_t lastSecondTickMs = 0;

String lastEvent = "System boot";
String lastFlowStopTime = "-";
String lastPumpTestTime = "-";
String lastFlowReturnTime = "-";
String lastAlarmTime = "-";

String mqttServer = "";
uint16_t mqttPort = MQTT_DEFAULT_PORT;
String mqttUser = "";
String mqttPassword = "";
String mqttTopic = MQTT_TOPIC_DEFAULT;

String adminUser = ADMIN_USER_DEFAULT;
String adminPass = ADMIN_PASS_DEFAULT;
