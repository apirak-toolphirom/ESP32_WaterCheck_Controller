#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pinmap.h"
#include "config.h"
#include "global.h"
#include "LCDDisplay.h"

static LiquidCrystal_I2C lcd(0x27, 16, 2);
static uint32_t lastLcd = 0;

static String fit16(const String& s) {
  if (s.length() >= 16) return s.substring(0, 16);
  String out = s;
  while (out.length() < 16) out += " ";
  return out;
}

void lcdBegin() {
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("WaterCheck Ready");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
}

void lcdUpdate() {
  uint32_t now = millis();
  if (now - lastLcd < LCD_UPDATE_MS) return;
  lastLcd = now;

  String line1 = state.dateText.substring(0,5) + " " + state.timeText.substring(0,5) + " P:" + (state.pumpOn ? "ON" : "OF");
  String line2 = state.counting ? ("Delay:" + String(state.remainSec) + "s") : (state.flowOn ? "Flow OK" : "No Flow");
  lcd.setCursor(0,0); lcd.print(fit16(line1));
  lcd.setCursor(0,1); lcd.print(fit16(line2));
}
