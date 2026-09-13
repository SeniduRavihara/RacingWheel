# 🔧 Troubleshooting

## ❌ LED blinks rapidly on startup
**Cause:** MPU-6050 not found on I2C bus  
**Fix:**
- Check SDA → GPIO 8, SCL → GPIO 9
- Check VCC → 3.3V (NOT 5V!)
- Check AD0 → GND
- Try `i2c_scanner` sketch to verify address (should be 0x68 or 0x69)

## ❌ PC doesn't recognize wheel as gamepad
**Cause:** USB mode not configured correctly  
**Fix:**
- Ensure `platformio.ini` has: `-D ARDUINO_USB_MODE=0`
- Use a **data-capable** USB-C cable (not charge-only)
- Windows: Open Device Manager → check for "Unknown Device"

## ❌ Steering goes only one direction
**Cause:** Gyro axis is wrong or inverted  
**Fix:**
- In `GyroSensor.cpp` change `gz` to `-gz` in the `update()` function

## ❌ Wheel drifts too much
**Fix:**
- Keep wheel still during boot calibration
- Reduce `DRIFT_CORRECTION` to 0.99 (more aggressive re-centering)
- Increase `COMPLEMENTARY_ALPHA` to 0.99 (trust gyro more)

## ❌ Wheel feels sluggish/delayed
**Fix:**
- Reduce `HID_REPORT_INTERVAL_MS` to 4 (250Hz)
- Make sure `Wire.setClock(400000)` is active (400kHz I2C)

## ❌ Buttons don't register
**Fix:**
- Check GPIO pin numbers match `Config.h`
- Firmware uses `INPUT_PULLUP` — wire buttons between GPIO and GND (NO external resistors needed)
