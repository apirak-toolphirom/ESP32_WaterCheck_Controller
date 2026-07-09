#include "global.h"
#include "config.h"

bool flowState = false;
bool pumpRelayState = false;
bool auxRelayState = false;

PumpState pumpState = PUMP_RUNNING;

uint32_t delayTimeSec = DEFAULT_DELAY_SEC;
uint32_t pumpTestTimeSec = DEFAULT_PUMP_TEST_SEC;
uint32_t remainTimeSec = 0;

uint32_t stateStartMs = 0;
uint32_t lastSecondTickMs = 0;

String mqttServer = "";
uint16_t mqttPort = MQTT_DEFAULT_PORT;
String mqttUser = "";
String mqttPassword = "";
String mqttTopic = MQTT_TOPIC_DEFAULT;

String adminUser = ADMIN_USER_DEFAULT;
String adminPass = ADMIN_PASS_DEFAULT;
