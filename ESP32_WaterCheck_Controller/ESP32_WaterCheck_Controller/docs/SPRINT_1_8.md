# Sprint 1.8

## ฟังก์ชันที่เพิ่ม

1. Wiring Diagram สี
2. Graph Dashboard
3. Telegram / LINE / Email Notification

## ดำเนินการแล้ว

### Wiring Diagram สี
- สร้าง `docs/images/wiring_diagram_color.svg` (เสร็จ)
- ระบุ GPIO: Flow, Reset, E-STOP, Relay, LCD, Power (เสร็จ)
- เพิ่มหมายเหตุความปลอดภัยระบบไฟฟ้า (เสร็จ)

### Graph Dashboard
- เพิ่ม `GraphHistory.h/.cpp` เก็บข้อมูลย้อนหลัง 60 จุด ทุก 5 วินาที (เสร็จ)
- เตรียม API `/api/graph` ผ่าน Patch (เสร็จ)
- เตรียม Patch Dashboard สำหรับกราฟ (เสร็จ)

### Telegram / LINE / Email
- เพิ่ม `NotificationConfig.h/.cpp` สำหรับบันทึก Token/Config (เสร็จ)
- เพิ่ม Telegram HTTPS sender (เสร็จ)
- เพิ่ม LINE Notify/compatible HTTPS sender (เสร็จ)
- เพิ่ม Email SMTP config structure (เสร็จ)
- Email SMTP actual sending ยังไม่ทำ ต้องใช้ Library เพิ่ม เช่น ESP_Mail_Client (ยังไม่ดำเนินการ)

## ยังไม่ได้ดำเนินการ

- รวม Patch เข้า `Dashboard.cpp` เต็ม
- รวม Patch เข้า `.ino` เต็ม
- Graph UI แบบ Chart สวยสมบูรณ์
- Email SMTP ส่งจริง
- LINE Messaging API แบบ Official Messaging API เต็มรูปแบบ
- Notification retry queue
- เก็บ Graph History ลง LittleFS
- Wiring Diagram PNG/PDF/DWG
- GitHub Actions Build Test

## Commit message แนะนำ

`feat(ux): add color wiring diagram graph history and notification channels`
