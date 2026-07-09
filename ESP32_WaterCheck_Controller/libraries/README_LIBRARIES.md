# Library Setup for ESP32 Water Check Controller

โปรเจกต์นี้ใช้ Arduino IDE + ESP32 Arduino Core

## 1) ติดตั้ง ESP32 Board Package

ใน Arduino IDE:

1. ไปที่ `File > Preferences`
2. ช่อง `Additional Boards Manager URLs` ใส่:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

3. ไปที่ `Tools > Board > Boards Manager`
4. ค้นหา `esp32`
5. ติดตั้ง `esp32 by Espressif Systems`

## 2) ติดตั้ง Library ผ่าน Library Manager

ไปที่ `Tools > Manage Libraries...` แล้วติดตั้ง:

| Library | Author | ใช้สำหรับ |
|---|---|---|
| WiFiManager | tzapu | ตั้งค่า WiFi ผ่านหน้า AP อัตโนมัติ |
| PubSubClient | Nick O'Leary | MQTT Client |
| ArduinoJson | Benoit Blanchon | JSON สำหรับ Dashboard และ MQTT |
| LiquidCrystal I2C | Frank de Brabander หรือ compatible | LCD 16x2 I2C |

## 3) Library ที่มากับ ESP32 Core อยู่แล้ว

ไม่ต้องติดตั้งเพิ่ม:

- WiFi.h
- WebServer.h
- Wire.h
- Preferences.h
- time.h
- esp_task_wdt.h

## 4) Arduino CLI ติดตั้งอัตโนมัติ

ถ้ามี Arduino CLI แล้ว สามารถรัน:

Windows:

```bat
tools\install_libs_arduino_cli.bat
```

macOS/Linux:

```bash
bash tools/install_libs_arduino_cli.sh
```

## 5) หมายเหตุ

ไม่ได้แนบไฟล์ Library ต้นฉบับไว้ใน ZIP เพราะ Library แต่ละตัวมีเวอร์ชันอัปเดตและ License ของตนเอง แนะนำติดตั้งจาก Arduino Library Manager หรือ Arduino CLI เพื่อให้ได้เวอร์ชันล่าสุดที่ถูกต้อง
