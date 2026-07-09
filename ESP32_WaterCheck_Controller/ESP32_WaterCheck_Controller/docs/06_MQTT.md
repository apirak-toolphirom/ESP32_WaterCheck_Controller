# MQTT

## Topic หลัก

ค่าเริ่มต้นคือ `water/check`

## Publish

| Topic | Payload | ใช้สำหรับ |
|---|---|---|
| water/check/json | JSON | สถานะรวม |
| water/check/alarm | Text | แจ้งเตือน Lockout |
| water/check/status | online/offline | LWT |

## Subscribe

| Topic | Command | ใช้สำหรับ |
|---|---|---|
| water/check/cmd | DELAY:300 | ตั้งค่า Delay |
| water/check/cmd | TEST:15 | ตั้งค่า Pump Test Time |
| water/check/cmd | RETRY:5 | ตั้งค่า Max Retry |
| water/check/cmd | PUMP_ON | Manual Pump ON |
| water/check/cmd | PUMP_OFF | Manual Pump OFF |
| water/check/cmd | UNLOCK | Reset Lockout |
| water/check/cmd | RESTART | Restart ESP32 |

สถานะ: อัปเดตแล้ว (เสร็จ)
