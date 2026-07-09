# Sprint 1.1 – Firmware Core

## สถานะงาน

- global.h (เสร็จ)
- globals.cpp (เสร็จ)
- ESP32_WaterCheck_v1.ino core logic (เสร็จ)
- Storage.cpp/.h Preferences Manager (เสร็จ)
- FlowSwitch.cpp/.h Debounce Input (เสร็จ)
- Relay.cpp/.h Pump Relay Driver (เสร็จ)
- Pump Retry Logic แบบตั้งค่า Pump Test Time ได้ (เสร็จ)

## Logic ใหม่

Flow OFF → ปิดปั๊ม → นับ Delay → เปิดปั๊มทดลองตาม Pump Test Time → ถ้ามี Flow ให้ทำงานต่อ → ถ้าไม่มี Flow ให้ปิดปั๊มและเริ่ม Delay รอบใหม่

## ไฟล์ที่ต้องอัปโหลดทับใน GitHub

อัปโหลดไฟล์ทั้งหมดในโฟลเดอร์นี้ไปที่:

`ESP32_WaterCheck_Controller/ESP32_WaterCheck_v1/`

## Commit message แนะนำ

`feat(core): add storage flow relay and adjustable pump test state machine`
