# Flow Logic และ Lockout Mode

## State

| State | ความหมาย | สถานะปั๊ม |
|---|---|---|
| PUMP_RUNNING | ปั๊มทำงานปกติ | ON |
| PUMP_DELAY_WAIT | น้ำไม่ไหล ปิดปั๊มและนับเวลาหน่วง | OFF |
| PUMP_TEST_RUNNING | เปิดปั๊มทดลองหลังครบเวลาหน่วง | ON |
| PUMP_LOCKOUT | ทดลองครบจำนวนครั้งแล้วยังไม่มีน้ำ | OFF |

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
Flow ON ? ─ Yes → Pump Running และ Reset Retry
  ↓ No
Retry ครบหรือยัง?
  ├─ No → Delay รอบใหม่
  └─ Yes → LOCKOUT + Alarm
```

## Unlock

ทำได้ 2 วิธี

1. กดปุ่ม `Unlock / Reset Alarm` ใน Dashboard
2. กดปุ่มหน้าเครื่องที่ `GPIO32` ค้าง 1.5 วินาที

สถานะ: อัปเดตแล้ว (เสร็จ)
