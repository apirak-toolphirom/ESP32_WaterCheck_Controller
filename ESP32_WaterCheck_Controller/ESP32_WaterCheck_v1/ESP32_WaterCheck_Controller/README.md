# ESP32 Water Check Controller V2.0

ระบบเช็คน้ำขาด by ช่างหนึ่ง

## Features
- ESP32 DevKit V1
- Flow Switch input on GPIO34
- Relay 2CH: pump on GPIO26, aux on GPIO27
- LCD 16x2 I2C on GPIO21/22
- NTP: time.nimt.or.th with pool.ntp.org fallback
- Web Dashboard with password-protected config
- MQTT configuration from dashboard
- Delay countdown after flow switch stops
- Preferences storage
- Non-blocking main loop

## Default Login
- User: admin
- Password: 123456

## WiFi Setup
On first boot, ESP32 creates AP: `WaterCheck_Setup`. Connect and enter WiFi credentials.

## Dashboard
Open the ESP32 IP address in browser.

## MQTT
Default base topic: `watercheck`

Publish:
- `watercheck/status`
- `watercheck/telemetry`

Subscribe:
- `watercheck/cmd`

Commands:
- `PUMP_ON`
- `PUMP_OFF`
- `AUX_ON`
- `AUX_OFF`
- `DELAY:300`
- `RESTART`

## Pin Map
| Function | GPIO |
|---|---:|
| Flow Switch | 34 |
| Relay Pump | 26 |
| Relay AUX | 27 |
| LCD SDA | 21 |
| LCD SCL | 22 |
| Status LED | 2 |

## Safety
Relay output controlling 220VAC pump must be wired by a qualified technician. Use fuse, breaker, enclosure, grounding, and contactor if pump current exceeds relay rating.

## Library Setup

เพิ่มโฟลเดอร์ `libraries/` และ `tools/` สำหรับติดตั้ง Library แล้ว

- อ่าน: `libraries/README_LIBRARIES.md`
- รายการ Library: `ESP32_WaterCheck_v1/libraries.txt`
- ติดตั้งผ่าน Arduino CLI: `tools/install_libs_arduino_cli.bat` หรือ `tools/install_libs_arduino_cli.sh`
- สำหรับ PlatformIO ใช้ไฟล์ `platformio.ini`
