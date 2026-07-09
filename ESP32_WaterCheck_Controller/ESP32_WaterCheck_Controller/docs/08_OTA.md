# OTA Firmware Update

## Web OTA

Dashboard มีเมนู Upload Firmware `.bin`

Endpoint:
- `POST /api/ota/upload`

หลังอัปโหลดสำเร็จ ESP32 จะ Restart อัตโนมัติ

สถานะ: เสร็จ

## ArduinoOTA

รองรับอัปเดตผ่าน Arduino IDE / Network Port

Hostname:
- `ESP32-WaterCheck`

สถานะ: เสร็จ

## OTA Partition Info

Endpoint:
- `GET /api/ota/info`

แสดง:
- running partition
- boot partition
- next OTA partition
- sketch size
- free sketch space

สถานะ: เสร็จ
