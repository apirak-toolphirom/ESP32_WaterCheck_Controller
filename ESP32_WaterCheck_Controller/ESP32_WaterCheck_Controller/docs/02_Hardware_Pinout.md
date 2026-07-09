# Hardware Pinout

| Function | GPIO | Status |
|---|---:|---|
| Flow Switch | 34 | เสร็จ |
| Local Reset / Unlock / Factory Reset | 32 | เสร็จ |
| E-STOP | 33 | เสร็จ |
| Relay Pump | 26 | เสร็จ |
| Relay AUX | 27 | เสร็จ |
| LCD SDA | 21 | เสร็จ |
| LCD SCL | 22 | เสร็จ |
| Status LED | 2 | เสร็จ |

## หมายเหตุ

- GPIO34 เป็น Input only เหมาะสำหรับ Flow Switch
- GPIO32 ใช้ปุ่ม Reset/Unlock และ Factory Reset
- GPIO33 ใช้ E-STOP
- ควรต่อ E-STOP ผ่านวงจรที่เหมาะสมกับงานไฟฟ้าจริง และควรมี Contactor/Breaker ฝั่งกำลังแยกจาก ESP32
