#include <Arduino.h>
#include <time.h>
#include "config.h"
#include "global.h"
#include "TimeSync.h"

void timeBegin() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_PRIMARY, NTP_SERVER_FALLBACK);
}

void timeUpdate() {
  static uint32_t last = 0;
  if (millis() - last < 1000) return;
  last = millis();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 20)) {
    char tbuf[9];
    char dbuf[11];
    strftime(tbuf, sizeof(tbuf), "%H:%M:%S", &timeinfo);
    strftime(dbuf, sizeof(dbuf), "%d/%m/%Y", &timeinfo);
    state.timeText = String(tbuf);
    state.dateText = String(dbuf);
  } else {
    state.timeText = "--:--:--";
    state.dateText = "--/--/----";
  }
}
