# CHANGELOG

## v0.1.4 - Sprint 1.4

### Added
- E-STOP button at GPIO33 (เสร็จ)
- Maintenance Mode (เสร็จ)
- Relay Fail Detection / no-flow timeout (เสร็จ)
- Factory Reset by holding local button GPIO32 for 10 seconds (เสร็จ)
- Manual Override AUTO / ON / OFF (เสร็จ)
- Watchdog reset protection (เสร็จ)
- Dashboard control buttons (เสร็จ)
- MQTT commands: `RELAYFAIL:<sec>`, `AUTO`, `MAINT_ON`, `MAINT_OFF` (เสร็จ)

### Changed
- State Machine extended with `PUMP_ESTOP` and `PUMP_MAINTENANCE` (เสร็จ)

### Not Done
- Persistent Event Log
- OTA
- CSV Export
- GitHub Actions Build Test
