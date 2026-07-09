# Safety Functions

## Emergency Stop (E-STOP)

- ใช้ GPIO33
- กดแล้วปั๊มหยุดทันที
- ระบบเข้าสู่ `PUMP_ESTOP`
- ปล่อยปุ่มแล้วระบบกลับไป `PUMP_DELAY_WAIT`

สถานะ: เสร็จ

## Maintenance Mode

- ใช้เมื่อต้องการซ่อมบำรุง
- ปิดการควบคุมปั๊มอัตโนมัติ
- ยังดูสถานะผ่าน Dashboard ได้

สถานะ: เสร็จ

## Relay Fail Detection

ระบบตรวจจับกรณีสั่งปั๊ม ON แต่ Flow ยังไม่กลับมาภายในเวลาที่กำหนด

Parameter:
- Relay Fail Timeout ค่าเริ่มต้น 30 วินาที
- ตั้งค่าได้ผ่าน Dashboard และ MQTT

สถานะ: เสร็จ

## Factory Reset

กดปุ่มหน้าเครื่อง GPIO32 ค้าง 10 วินาที เพื่อล้าง:
- WiFiManager settings
- Preferences ทั้งหมด
- MQTT config
- Delay/Pump Test/Retry config

สถานะ: เสร็จ
