# Sprint 1.3

## ดำเนินการแล้ว

- Pump Retry Count ตั้งค่าได้ (เสร็จ)
- Lockout Mode เมื่อครบจำนวนครั้งแล้วยังไม่มีน้ำ (เสร็จ)
- MQTT Alarm แจ้งเตือน Lockout (เสร็จ)
- Dashboard Alarm แสดงสถานะสีแดงและปุ่ม Unlock (เสร็จ)
- History Log เบื้องต้น บันทึกเหตุการณ์สำคัญ (เสร็จ)
- ปุ่ม Reset/Unlock หน้าเครื่อง GPIO32 กดค้าง 1.5 วินาที (เสร็จ)
- รวม WiFiManager, NTP, LCD, MQTT และ Dashboard เข้ากับ main firmware (เสร็จ)

## รายการที่ยังไม่ได้ดำเนินการ

- OTA Update
- Event Log แบบเก็บถาวรใน SPIFFS/LittleFS
- Export Log
- Graph Dashboard
- Manual Override แบบเต็ม
- Watchdog
- Unit Test / Integration Test อัตโนมัติ
- Wiring Diagram แบบรูปภาพ
- คู่มือ Dashboard ฉบับละเอียด
- คู่มือ MQTT ฉบับละเอียด
- ระบบแจ้งเตือน Line/Telegram
- Build test ด้วย GitHub Actions

## Commit message แนะนำ

`feat(field): add lockout retry alarm dashboard unlock and local reset button`
