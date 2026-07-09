# Sprint 1.4

## ดำเนินการแล้ว

- Emergency Stop (E-STOP) GPIO33 (เสร็จ)
- Maintenance Mode ผ่าน Dashboard/MQTT (เสร็จ)
- Relay Fail Detection / No-flow after pump ON (เสร็จ)
- Factory Reset ด้วยปุ่มหน้าเครื่อง GPIO32 กดค้าง 10 วินาที (เสร็จ)
- Manual Override AUTO / ON / OFF ผ่าน Dashboard/MQTT (เสร็จ)
- Watchdog 10 วินาที (เสร็จ)
- Dashboard ปุ่ม Control เพิ่มเติม (เสร็จ)
- LCD แสดง ESTOP / MAINT / RELAY FAIL (เสร็จ)
- MQTT command เพิ่มเติม (เสร็จ)

## รายการที่ยังไม่ได้ดำเนินการ

- OTA Update
- Event Log ถาวรลง LittleFS
- Export CSV
- Graph Dashboard
- Wiring Diagram รูปภาพ
- Build Test ด้วย GitHub Actions
- LINE/Telegram Notification
- User Management หลายระดับ
- HTTPS Dashboard
- Hardware relay feedback input จริง

## Commit message แนะนำ

`feat(safety): add estop maintenance relay fail factory reset and watchdog`
