# Wiring Guide

## ESP32 to LCD 16x2 I2C
| LCD | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

## ESP32 to Flow Switch
| Flow Switch | ESP32 |
|---|---|
| Signal | GPIO34 |
| GND | GND |

If the flow switch output is open collector or dry contact, add suitable pull-up/pull-down circuitry.

## ESP32 to Relay Module 2CH
| Relay | ESP32 |
|---|---|
| IN1 | GPIO26 |
| IN2 | GPIO27 |
| VCC | 5V |
| GND | GND |

Use common GND between ESP32 and relay module.

## AC Pump Wiring
Use relay/contact output to drive a contactor coil for large pump loads. Do not drive high-current pump directly from low-cost relay board unless relay rating and protection are verified.
