#include "TimeSync.h"
#include "config.h"
#include <time.h>

void timeSyncBegin() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
}

bool timeIsReady() {
  struct tm timeinfo;
  return getLocalTime(&timeinfo, 50);
}

String timeNowString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return "--:--:--";
  char buf[16];
  strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
  return String(buf);
}

String dateTimeNowString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return "-";
  char buf[24];
  strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &timeinfo);
  return String(buf);
}
