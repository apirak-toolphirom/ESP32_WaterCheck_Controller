# Event Log Manager

## Storage

ใช้ LittleFS เก็บไฟล์:

- `/events.csv`
- `/events_old.csv`

## CSV Format

```csv
timestamp,type,message
"09/07/2026 08:00:00","EVENT","Pump running"
```

## API ที่ควรเพิ่มใน Dashboard

| Endpoint | Method | ใช้สำหรับ |
|---|---|---|
| /api/events.csv | GET | ดาวน์โหลด CSV |
| /api/events.json | GET | อ่าน Log เป็น JSON |
| /api/events/clear | POST | ล้าง Log |

สถานะ: ตัวจัดการ Log เสร็จแล้ว แต่ Dashboard ต้อง patch เพิ่ม
