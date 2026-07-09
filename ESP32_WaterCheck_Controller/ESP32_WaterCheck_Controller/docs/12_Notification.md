# Notification

## Sprint 1.6

ทำระบบกลางสำหรับแจ้งเตือนแล้ว:

```cpp
notifyAlarm("LOCKOUT", "max retry reached");
notifyEvent("FLOW", "flow stopped");
```

## ช่องทางที่ทำแล้ว

- Event Log (เสร็จ)
- MQTT Alarm (เสร็จ)

## ยังไม่ได้ดำเนินการ

- Telegram
- LINE Messaging API
- Email
- Retry Queue
- Notification Config Dashboard
