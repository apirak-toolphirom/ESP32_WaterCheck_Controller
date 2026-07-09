# MQTT Commands

Base topic: `water/check`

## Subscribe

`water/check/cmd`

| Command | Function | Status |
|---|---|---|
| DELAY:300 | ตั้ง Delay | เสร็จ |
| TEST:15 | ตั้ง Pump Test Time | เสร็จ |
| RETRY:5 | ตั้ง Max Retry | เสร็จ |
| RELAYFAIL:30 | ตั้ง Relay Fail Timeout | เสร็จ |
| PUMP_ON | Manual ON | เสร็จ |
| PUMP_OFF | Manual OFF | เสร็จ |
| AUTO | กลับ Auto Mode | เสร็จ |
| MAINT_ON | Maintenance ON | เสร็จ |
| MAINT_OFF | Maintenance OFF | เสร็จ |
| UNLOCK | Reset Alarm | เสร็จ |
| RESTART | Restart ESP32 | เสร็จ |
