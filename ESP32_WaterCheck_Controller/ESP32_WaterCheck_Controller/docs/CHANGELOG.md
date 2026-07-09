# CHANGELOG

## v0.1.6 - Sprint 1.6

### Added
- EventLog manager using LittleFS (เสร็จ)
- CSV event log file `/events.csv` (เสร็จ)
- Basic log rotation to `/events_old.csv` (เสร็จ)
- Export CSV helper (เสร็จ)
- Export JSON helper (เสร็จ)
- Clear log helper (เสร็จ)
- Notification manager (เสร็จ)
- notifyAlarm(): EventLog + MQTT alarm (เสร็จ)
- notifyEvent(): EventLog only (เสร็จ)

### Patch Required
- Dashboard.cpp endpoint patch
- ESP32_WaterCheck_v1.ino event log integration patch

### Not Done
- Telegram
- LINE Messaging API
- Email
- Dashboard table viewer
- Date search/filter
