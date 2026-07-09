# Sprint 1.5

## ฟังก์ชันที่ดำเนินการ

พักไว้:
- RTC Module Backup DS3231 (พักไว้ก่อน)

ดำเนินการแล้ว:
- Dual Firmware OTA / OTA Partition Info (เสร็จ)
- OTA ผ่าน Web Dashboard upload `.bin` (เสร็จ)
- OTA ผ่าน ArduinoOTA (เสร็จ)
- Health Monitor: uptime, heap, min heap, WiFi RSSI, flash/sketch space, chip temp (เสร็จ)
- Configuration Backup / Restore เป็น JSON ผ่าน Dashboard (เสร็จ)
- Dashboard เพิ่ม Health Monitor, OTA Upload, Export/Import Config (เสร็จ)

## รายการที่ยังไม่ได้ดำเนินการ

- RTC DS3231 Backup (พักไว้ก่อน)
- Event Log ถาวรลง LittleFS
- Export Log CSV/JSON
- Graph Dashboard
- Wiring Diagram รูปภาพ
- GitHub Actions Build Test
- LINE/Telegram Notification
- HTTPS Dashboard
- TLS MQTT
- User Management หลายระดับ

## Commit message แนะนำ

`feat(ops): add ota health monitor and config backup restore`
