# ESP32 WaterCheck Sprint 1.6 Package

## ฟังก์ชันที่ทำใน Sprint นี้

- Event Log Manager ด้วย LittleFS (เสร็จ)
- Notification Manager เชื่อม EventLog + MQTT Alarm (เสร็จ)

## วิธีอัปโหลด

อัปโหลดไฟล์ใน:

`ESP32_WaterCheck_Controller/ESP32_WaterCheck_v1/`

ไปยังโฟลเดอร์ firmware เดิม

อัปโหลด docs ไปยังโฟลเดอร์ `docs/`

## หมายเหตุ

Sprint นี้มีไฟล์ Patch:
- `Dashboard_EventLog_Patch.txt`
- `Main_INO_Patch.txt`

ถ้าต้องการ ผมจะทำ Sprint 1.6.1 เพื่อรวม Patch ทั้งหมดเป็นไฟล์ `.ino` และ `Dashboard.cpp` เต็มให้

## Commit message

`feat(log): add littlefs event log and notification manager`
