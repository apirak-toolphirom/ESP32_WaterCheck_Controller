# Patch: Adjustable Pump Test Time

เพิ่มค่าตั้งเวลา `pumpTestSec` สำหรับเวลาทดลองเดินปั๊มหลังครบ Delay

## Logic ใหม่
1. Flow หยุด -> ปิดปั๊ม -> เริ่ม Delay countdown
2. ครบ Delay -> เปิดปั๊มทดลองตาม `pumpTestSec`
3. ถ้ามี Flow กลับมา -> คงปั๊ม ON และยกเลิก countdown/test
4. ถ้ายังไม่มี Flow จนครบ `pumpTestSec` -> ปิดปั๊มและเริ่ม Delay รอบใหม่

## ตั้งค่าได้จาก
- Web Dashboard: ช่อง “เวลาทดลองเดินปั๊มหลังครบ Delay”
- MQTT command: `TEST:<seconds>`

## ค่าเริ่มต้น
- DEFAULT_PUMP_TEST_SEC = 20 วินาที
- MIN = 1 วินาที
- MAX = 3600 วินาที
