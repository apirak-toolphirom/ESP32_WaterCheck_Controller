# Test Plan

1. Upload firmware to ESP32.
2. Connect to `WaterCheck_Setup` AP and set WiFi.
3. Open ESP32 IP.
4. Verify dashboard title: ระบบเช็คน้ำขาด by ช่างหนึ่ง.
5. Login with admin / 123456.
6. Set delay to 30 seconds.
7. Toggle flow switch ON: pump should ON and countdown reset.
8. Toggle flow switch OFF: pump should OFF and countdown starts.
9. After countdown ends: pump should ON again.
10. Configure MQTT and subscribe to telemetry topic.
11. Check LCD line 1 shows date/time and pump status.
12. Check LCD line 2 shows countdown or ready state.
