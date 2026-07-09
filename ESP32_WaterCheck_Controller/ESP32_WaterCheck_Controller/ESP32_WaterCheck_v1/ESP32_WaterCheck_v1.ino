#include <Arduino.h>
#include "config.h"
#include "pinmap.h"
#include "global.h"
#include "Storage.h"
#include "Relay.h"
#include "FlowSwitch.h"

// โมดูล LCD / MQTT / Dashboard จาก Sprint ถัดไปจะถูก include เพิ่มภายหลัง
// #include "LCDDisplay.h"
// #include "MQTTClient.h"
// #include "Dashboard.h"

static void enterState(PumpState nextState) {
  pumpState = nextState;
  stateStartMs = millis();

  switch (pumpState) {
    case PUMP_RUNNING:
      setPump(true);
      remainTimeSec = 0;
      break;

    case PUMP_DELAY_WAIT:
      setPump(false);
      remainTimeSec = delayTimeSec;
      break;

    case PUMP_TEST_RUNNING:
      setPump(true);
      remainTimeSec = pumpTestTimeSec;
      break;
  }
}

static uint32_t elapsedSeconds() {
  return (millis() - stateStartMs) / 1000UL;
}

static void updateCountdown() {
  uint32_t now = millis();
  if (now - lastSecondTickMs < 1000UL) return;
  lastSecondTickMs = now;

  uint32_t target = 0;
  if (pumpState == PUMP_DELAY_WAIT) target = delayTimeSec;
  if (pumpState == PUMP_TEST_RUNNING) target = pumpTestTimeSec;

  if (target == 0) {
    remainTimeSec = 0;
    return;
  }

  uint32_t elapsed = elapsedSeconds();
  remainTimeSec = (elapsed >= target) ? 0 : (target - elapsed);
}

static void updatePumpLogic() {
  flowUpdate();

  switch (pumpState) {
    case PUMP_RUNNING:
      // ปั๊มทำงานปกติ ถ้า Flow หาย ให้หยุดปั๊มแล้วเริ่มหน่วงเวลา
      if (!flowIsOn()) {
        enterState(PUMP_DELAY_WAIT);
      }
      break;

    case PUMP_DELAY_WAIT:
      // ถ้า Flow กลับมาก่อนครบเวลา ให้กลับสู่สถานะทำงานปกติ
      if (flowIsOn()) {
        enterState(PUMP_RUNNING);
        break;
      }

      // ครบเวลาหน่วง ให้เปิดปั๊มทดลอง
      if (elapsedSeconds() >= delayTimeSec) {
        enterState(PUMP_TEST_RUNNING);
      }
      break;

    case PUMP_TEST_RUNNING:
      // หากเริ่มมีน้ำไหลระหว่างช่วงทดสอบ ให้ปั๊มทำงานต่อ
      if (flowIsOn()) {
        enterState(PUMP_RUNNING);
        break;
      }

      // หากครบเวลาทดสอบแล้วยังไม่มีน้ำ ให้ปิดปั๊มและรอรอบถัดไป
      if (elapsedSeconds() >= pumpTestTimeSec) {
        enterState(PUMP_DELAY_WAIT);
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  storageBegin();
  storageLoad();

  relayBegin();
  flowBegin();

  enterState(PUMP_RUNNING);

  Serial.println();
  Serial.println("ESP32 Water Check Controller");
  Serial.print("Firmware: ");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Delay Time: ");
  Serial.print(delayTimeSec);
  Serial.println(" sec");
  Serial.print("Pump Test Time: ");
  Serial.print(pumpTestTimeSec);
  Serial.println(" sec");
}

void loop() {
  updatePumpLogic();
  updateCountdown();

  // สถานะ LED: ON เมื่อปั๊มทำงาน
  digitalWrite(STATUS_LED_PIN, pumpRelayState ? HIGH : LOW);

  // Sprint ถัดไปจะเพิ่ม:
  // lcdUpdate();
  // mqttLoop();
  // dashboardLoop();

  delay(2);
}
