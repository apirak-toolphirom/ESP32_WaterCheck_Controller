# Configuration Backup / Restore

## Export

Endpoint:
- `GET /api/config/export`

ดาวน์โหลดไฟล์:
- `watercheck_config.json`

ข้อมูลที่สำรอง:
- Delay Time
- Pump Test Time
- Max Retry
- Relay Fail Timeout
- MQTT Server / Port / User / Password / Topic
- Dashboard Username / Password

สถานะ: เสร็จ

## Import

Endpoint:
- `POST /api/config/import`

ส่ง JSON กลับเข้า ESP32 เพื่อกู้คืนค่าตั้งต้น

สถานะ: เสร็จ
