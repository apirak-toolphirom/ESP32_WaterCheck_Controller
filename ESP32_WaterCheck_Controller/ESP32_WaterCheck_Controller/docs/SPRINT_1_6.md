# Sprint 1.6

## ฟังก์ชันที่เลือกดำเนินการ

ดำเนินการ:
1. Event Log Manager (LittleFS)
4. Notification

## ดำเนินการแล้ว

### Event Log Manager
- EventLog.h (เสร็จ)
- EventLog.cpp (เสร็จ)
- LittleFS begin/mount (เสร็จ)
- CSV Log File `/events.csv` (เสร็จ)
- Log Rotation เมื่อไฟล์เกิน `EVENT_LOG_MAX_BYTES` (เสร็จ)
- Export CSV ผ่าน Dashboard endpoint patch (เสร็จ)
- Export JSON ผ่าน Dashboard endpoint patch (เสร็จ)
- Clear Log endpoint patch (เสร็จ)

### Notification
- Notification.h (เสร็จ)
- Notification.cpp (เสร็จ)
- notifyAlarm() บันทึก Event Log + MQTT Alarm (เสร็จ)
- notifyEvent() บันทึก Event Log (เสร็จ)

## ยังไม่ได้ดำเนินการ

### Event Log
- Search/filter ตามวันที่
- Event Log Viewer แบบตารางใน Dashboard เต็มรูปแบบ
- เก็บ 1000 events แบบ ring buffer จริง
- บีบอัด log
- Export JSON พร้อม pagination

### Notification
- Telegram
- LINE Messaging API
- Email
- Config หน้า Dashboard สำหรับ token/chat id
- Notification retry queue เมื่อเน็ตหลุด

### Dashboard Professional
- Sidebar
- Dark Mode
- Mobile UI เต็มรูปแบบ
- Graph Dashboard

### Security
- Session Login
- Logout
- Password Change
- HTTPS Dashboard
- Config Backup Encryption

### Testing/CI
- GitHub Actions Build Test
- Unit Test
- Integration Test

## Commit message แนะนำ

`feat(log): add littlefs event log and notification manager`
