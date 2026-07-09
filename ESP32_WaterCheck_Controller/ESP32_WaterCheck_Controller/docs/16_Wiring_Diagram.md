# Wiring Diagram สี

ไฟล์:
- `docs/images/wiring_diagram_color.svg`

## Pinout

| Function | GPIO | Status |
|---|---:|---|
| Flow Switch | 34 | เสร็จ |
| Reset/Unlock/Factory Reset | 32 | เสร็จ |
| E-STOP | 33 | เสร็จ |
| Relay Pump | 26 | เสร็จ |
| Relay AUX | 27 | เสร็จ |
| LCD SDA | 21 | เสร็จ |
| LCD SCL | 22 | เสร็จ |
| Status LED | 2 | เสร็จ |

## หมายเหตุ

E-STOP สำหรับงานจริงควรตัดวงจรกำลังผ่านอุปกรณ์ไฟฟ้า เช่น Contactor หรือ Safety Relay ไม่ควรพึ่ง ESP32 อย่างเดียว
