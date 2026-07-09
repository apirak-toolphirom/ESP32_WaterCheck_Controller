#include "LCDDisplay.h"
#include "global.h"
#include "pinmap.h"
#include "TimeSync.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);
static uint32_t lastLcdMs = 0;

void lcdBegin() {
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WaterCheck");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
}

static String stateText() {
  switch (pumpState) {
    case PUMP_RUNNING: return "RUN";
    case PUMP_DELAY_WAIT: return "WAIT";
    case PUMP_TEST_RUNNING: return "TEST";
    case PUMP_LOCKOUT: return "LOCK";
  }
  return "UNK";
}

void lcdUpdate() {
  uint32_t now = millis();
  if (now - lastLcdMs < 500) return;
  lastLcdMs = now;

  String line1 = timeNowString() + " P:" + String(pumpRelayState ? "ON" : "OF");
  String line2;
  if (pumpState == PUMP_LOCKOUT) {
    line2 = "LOCKOUT R:" + String(currentRetryCount);
  } else {
    line2 = stateText() + " " + String(remainTimeSec) + "s R:" + String(currentRetryCount);
  }

  while (line1.length() < 16) line1 += " ";
  while (line2.length() < 16) line2 += " ";
  line1 = line1.substring(0, 16);
  line2 = line2.substring(0, 16);

  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
