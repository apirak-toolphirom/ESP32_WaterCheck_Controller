# Sprint 1.6.1

## เป้าหมาย

รวม Patch จาก Sprint 1.6 ให้เป็นไฟล์เต็ม

## ดำเนินการแล้ว

- รวม EventLog เข้ากับ `ESP32_WaterCheck_v1.ino` (เสร็จ)
- รวม Notification เข้ากับ `ESP32_WaterCheck_v1.ino` (เสร็จ)
- รวม Dashboard Event Log endpoints เข้า `Dashboard.cpp` (เสร็จ)
- เพิ่ม Dashboard Event Log Viewer แบบตารางเบื้องต้น (เสร็จ)
- เพิ่มปุ่ม Download CSV / Refresh Log / Clear Log ใน Dashboard (เสร็จ)

## ยังไม่ได้ดำเนินการ

- Filter Event Log ตามวันที่
- Pagination Event Log
- Event Log ring buffer 1000 รายการแบบสมบูรณ์
- Telegram / LINE / Email
- GitHub Actions Build Test
- Wiring Diagram รูปภาพ

## Commit message แนะนำ

`chore(integration): merge event log and notification patches`
