#ifndef CONFIG_H
#define CONFIG_H

#define DASHBOARD_TITLE "ระบบเช็คน้ำขาด by ช่างหนึ่ง"

#define ADMIN_USER_DEFAULT "admin"
#define ADMIN_PASS_DEFAULT "123456"

#define NTP_SERVER_PRIMARY "time.nimt.or.th"
#define NTP_SERVER_FALLBACK "pool.ntp.org"
#define GMT_OFFSET_SEC 25200
#define DAYLIGHT_OFFSET_SEC 0

#define DEFAULT_DELAY_SEC 300UL
#define MIN_DELAY_SEC 1UL
#define MAX_DELAY_SEC 86400UL

// Pump test run time after delay expires.
// When no water flow is detected, the pump stays OFF for delaySec.
// After that, the pump turns ON for pumpTestSec to check whether water returns.
#define DEFAULT_PUMP_TEST_SEC 20UL
#define MIN_PUMP_TEST_SEC 1UL
#define MAX_PUMP_TEST_SEC 3600UL

#define MQTT_DEFAULT_PORT 1883
#define MQTT_CLIENT_PREFIX "ESP32WaterCheck-"
#define MQTT_BASE_TOPIC_DEFAULT "watercheck"

#define FLOW_DEBOUNCE_MS 200UL
#define LCD_UPDATE_MS 1000UL
#define MQTT_PUBLISH_MS 5000UL
#define DASHBOARD_UPDATE_MS 1000UL
#define MQTT_RECONNECT_MS 5000UL
#define WIFI_RESTART_AFTER_MS 300000UL

#define RELAY_ACTIVE_LOW 1

#endif
