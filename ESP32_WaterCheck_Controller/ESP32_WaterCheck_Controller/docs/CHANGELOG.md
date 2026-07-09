# CHANGELOG

## v0.1.1 - Sprint 1.1

### Added
- global.h และ globals.cpp สำหรับตัวแปรกลางของระบบ (เสร็จ)
- Storage Manager ด้วย Preferences (เสร็จ)
- Flow Switch debounce input (เสร็จ)
- Relay driver สำหรับ Pump และ AUX (เสร็จ)
- Adjustable Pump Test Time หลังครบ Delay (เสร็จ)
- Firmware core state machine ใน ESP32_WaterCheck_v1.ino (เสร็จ)

### Changed
- ปรับ Logic จากเดิมที่ครบ Delay แล้ว ON ทันที เป็นระบบ Pump Test Time ที่ตั้งค่าได้ (เสร็จ)

### Next
- Sprint 1.2: LCD, NTP, MQTT skeleton และ Dashboard API
