# 🧪 Hardware Test Suite

Component-by-component hardware tests for the ESP32-S3 DIY Racing Wheel project.

> 📘 **Full Arduino IDE & Upload Guide:** See [docs/arduino_ide_guide.md](../docs/arduino_ide_guide.md)

---

## ⚙️ Quick Arduino IDE Settings

Select under **Tools** in Arduino IDE before uploading:
- **Board:** `ESP32S3 Dev Module`
- **Flash Size:** `16MB (128Mb)`
- **PSRAM:** `OPI PSRAM`
- **USB Mode:** `USB-OTG (TinyUSB)` ⚠️ *(Required for Keyboard / Gamepad)*
- **USB CDC On Boot:** `Enabled`

---

## 🔌 Wiring Table (ESP32-S3 to MPU-6050 & Buttons)

| Component | Component Pin | ESP32-S3 Pin | Note |
|-----------|---------------|--------------|------|
| **MPU-6050** | **VCC** | **3.3V** | ⚠️ Connect to 3.3V, NOT 5V! |
| **MPU-6050** | **GND** | **GND** | Ground |
| **MPU-6050** | **SDA** | **GPIO 8** | I2C Data |
| **MPU-6050** | **SCL** | **GPIO 9** | I2C Clock |
| **Button (Gas / W)** | Pin 1 | **GPIO 4** | Pin 2 -> GND |
| **Button (Brake / S)**| Pin 1 | **GPIO 5** | Pin 2 -> GND |
| **Button (Steer Left / A)**| Pin 1 | **GPIO 6** | Pin 2 -> GND |
| **Button (Steer Right / D)**| Pin 1 | **GPIO 7** | Pin 2 -> GND |

---

## 📋 Available Tests

### Test 1: I2C Scanner (`01_i2c_scanner`)
- **Location:** `tests/01_i2c_scanner/01_i2c_scanner.ino`
- **Purpose:** Scans the I2C bus to verify ESP32-S3 recognizes the MPU-6050 module (Address `0x68`).

### Test 2: MPU-6050 Live Gyro & Steering Test (`02_mpu6050_test`)
- **Location:** `tests/02_mpu6050_test/02_mpu6050_test.ino`
- **Purpose:** Calculates real-time steering angle and prints a visual steering bar in Serial Monitor.

### Test 3: USB HID Keyboard Simulation (`03_keyboard_test`)
- **Location:** `tests/03_keyboard_test/03_keyboard_test.ino`
- **Purpose:** Simulates a native USB Keyboard on your PC.
  - Works **with or without** Gyro attached.
  - Auto-types `"ESP32-S3 Keyboard Working!"` every 3s if no gyro is plugged in.
  - If Gyro attached: Steering Left/Right types **`A`** / **`D`**.
  - Shorting GPIO 4, 5, 6, 7 to GND types **`W`**, **`S`**, **`A`**, **`D`**.
