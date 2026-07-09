# CHANGELOG

## v0.1.3 - Sprint 1.3

### Added
- Pump Retry Count ตั้งค่าได้ผ่าน Dashboard และ MQTT (`RETRY:<count>`) (เสร็จ)
- Lockout Mode เมื่อทดลองเดินปั๊มครบจำนวนครั้งแล้วยังไม่มีน้ำ (เสร็จ)
- MQTT Alarm topic สำหรับแจ้งเตือน Lockout (เสร็จ)
- Dashboard Alarm พร้อมปุ่ม Unlock / Reset Alarm (เสร็จ)
- History Log เบื้องต้น: Flow Stop, Pump Test, Flow Return, Alarm (เสร็จ)
- Local Reset/Unlock Button ที่ GPIO32 กดค้าง 1.5 วินาที (เสร็จ)
- LCD แสดง State, Countdown และ Retry (เสร็จ)
- NTP ผ่าน `time.nimt.or.th` พร้อม fallback `pool.ntp.org` (เสร็จ)

### Changed
- ขยาย State Machine เป็น `PUMP_RUNNING`, `PUMP_DELAY_WAIT`, `PUMP_TEST_RUNNING`, `PUMP_LOCKOUT` (เสร็จ)

### Not Done
- OTA Update
- Persistent Event Log
- Export Log
- GitHub Actions Build Test
- Wiring Diagram Image
