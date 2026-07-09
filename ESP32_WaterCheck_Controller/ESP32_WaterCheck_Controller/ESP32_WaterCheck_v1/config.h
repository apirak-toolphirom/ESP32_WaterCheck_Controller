#ifndef CONFIG_H
#define CONFIG_H

#define FIRMWARE_VERSION "0.1.3-sprint1.3"
#define DASHBOARD_TITLE "ระบบเช็คน้ำขาด by ช่างหนึ่ง"

#define ADMIN_USER_DEFAULT "admin"
#define ADMIN_PASS_DEFAULT "123456"

#define NTP_SERVER1 "time.nimt.or.th"
#define NTP_SERVER2 "pool.ntp.org"
#define GMT_OFFSET_SEC 25200
#define DAYLIGHT_OFFSET_SEC 0

#define DEFAULT_DELAY_SEC 300
#define MIN_DELAY_SEC 1
#define MAX_DELAY_SEC 86400

#define DEFAULT_PUMP_TEST_SEC 15
#define MIN_PUMP_TEST_SEC 1
#define MAX_PUMP_TEST_SEC 300

#define DEFAULT_MAX_RETRY 5
#define MIN_MAX_RETRY 1
#define MAX_MAX_RETRY 50

#define MQTT_DEFAULT_PORT 1883
#define MQTT_TOPIC_DEFAULT "water/check"

#endif
