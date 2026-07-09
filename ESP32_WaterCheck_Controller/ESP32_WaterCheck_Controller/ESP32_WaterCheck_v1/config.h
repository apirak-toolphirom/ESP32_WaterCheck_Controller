#ifndef CONFIG_H
#define CONFIG_H

#define FIRMWARE_VERSION "0.1.6-sprint1.6"
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

#define DEFAULT_RELAY_FAIL_SEC 30
#define MIN_RELAY_FAIL_SEC 5
#define MAX_RELAY_FAIL_SEC 600

#define LOCAL_UNLOCK_HOLD_MS 1500
#define FACTORY_RESET_HOLD_MS 10000

#define MQTT_DEFAULT_PORT 1883
#define MQTT_TOPIC_DEFAULT "water/check"

#define HEALTH_INTERVAL_MS 5000

#define EVENT_LOG_FILE "/events.csv"
#define EVENT_LOG_MAX_BYTES 65536
#define EVENT_LOG_BACKUP_FILE "/events_old.csv"

#endif
