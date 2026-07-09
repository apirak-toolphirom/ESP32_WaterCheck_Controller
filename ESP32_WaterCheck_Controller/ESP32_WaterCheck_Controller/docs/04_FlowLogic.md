# Pump Retry Logic

## สถานะระบบ

1. `PUMP_RUNNING` — ปั๊มทำงานปกติ
2. `PUMP_DELAY_WAIT` — ปิดปั๊มและนับเวลาหน่วง
3. `PUMP_TEST_RUNNING` — เปิดปั๊มทดลองตามเวลาที่ตั้งค่า

## Flow

```text
Flow OFF
  ↓
Pump OFF
  ↓
Delay Countdown
  ↓
Pump Test ON
  ↓
Flow ON ? ─ Yes → Pump Running
  ↓ No
Pump OFF and restart Delay
```

## Parameter

| Parameter | Default | Min | Max | ใช้สำหรับ |
|---|---:|---:|---:|---|
| Delay Time | 300 วินาที | 1 | 86400 | หน่วงเวลาก่อนทดลองเดินปั๊ม |
| Pump Test Time | 15 วินาที | 1 | 300 | เปิดปั๊มทดลองเพื่อตรวจว่าน้ำกลับมาหรือยัง |

สถานะ: เอกสารนี้อัปเดตแล้ว (เสร็จ)
