# Firmware — ESP32-S3 Racing Wheel

## Setup

1. Install [PlatformIO](https://platformio.org/) (VS Code extension recommended)
2. Open the `firmware/` folder in VS Code
3. Edit `src/config/Config.h` to match your GPIO wiring
4. Build & upload: `pio run --target upload`

## File Overview

| File | Purpose |
|------|---------|
| `src/main.cpp` | Setup + main loop |
| `src/config/Config.h` | All pins & tuning values |
| `src/gyro/GyroSensor.*` | MPU-6050 driver + angle integration |
| `src/hid/UsbHidGamepad.*` | USB HID gamepad output |
| `src/buttons/ButtonMatrix.*` | Button debounce handler |
| `platformio.ini` | Board & build configuration |

## Calibration
On every power-on, the gyro auto-calibrates for 1 second.  
**Keep the wheel completely still** during the LED blink at startup!

## Debug Mode
Uncomment `#define DEBUG_SERIAL` in `Config.h` to see live angle values in Serial Monitor.
