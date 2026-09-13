# 🔌 Wiring Guide

## MPU-6050 → ESP32-S3

| MPU-6050 Pin | ESP32-S3 Pin | Notes |
|-------------|-------------|-------|
| VCC | 3.3V | Do NOT use 5V — will damage ESP32 |
| GND | GND | |
| SDA | GPIO 8 | I2C Data |
| SCL | GPIO 9 | I2C Clock |
| AD0 | GND | Sets I2C address to 0x68 |
| INT | GPIO 4 (optional) | Interrupt pin (not required) |

## Buttons → ESP32-S3

| Button | ESP32-S3 Pin | Wiring |
|--------|-------------|--------|
| Gear Up | GPIO 4 | Button → GPIO, other end → GND |
| Gear Down | GPIO 5 | Button → GPIO, other end → GND |
| Button A | GPIO 6 | Button → GPIO, other end → GND |
| Button B | GPIO 7 | Button → GPIO, other end → GND |
| Button X | GPIO 15 | Button → GPIO, other end → GND |
| Button Y | GPIO 16 | Button → GPIO, other end → GND |

> Firmware uses `INPUT_PULLUP` — no external resistors needed for buttons!

## Status LED

| LED Pin | ESP32-S3 Pin | Notes |
|---------|-------------|-------|
| Anode (+) | GPIO 2 | Via 330Ω resistor |
| Cathode (–) | GND | |

## LED Status Meanings

| LED Pattern | Meaning |
|------------|---------|
| Rapid blink | ERROR — MPU-6050 not found |
| Solid ON (1 sec) | Calibrating — keep wheel still! |
| Solid ON | Ready to race ✅ |
