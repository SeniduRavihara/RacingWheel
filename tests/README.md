# 🧪 Hardware Test Suite

Component-by-component hardware tests for the ESP32-S3 DIY Racing Wheel project.

---

## 🔌 Wiring Table (ESP32-S3 to MPU-6050 & Buttons)

| Component | Component Pin | ESP32-S3 Pin | Note |
|-----------|---------------|--------------|------|
| **MPU-6050** | **VCC** | **3.3V** | ⚠️ Connect to 3.3V, NOT 5V! |
| **MPU-6050** | **GND** | **GND** | Ground |
| **MPU-6050** | **SDA** | **GPIO 8** | I2C Data |
| **MPU-6050** | **SCL** | **GPIO 9** | I2C Clock |
| **Button (Gas)** | Pin 1 | **GPIO 4** | Pin 2 -> GND |
| **Button (Brake)**| Pin 1 | **GPIO 5** | Pin 2 -> GND |
| **Button (Handbrake)**| Pin 1 | **GPIO 6** | Pin 2 -> GND |

---

## 📋 Test 1: I2C Scanner (`01_i2c_scanner`)
- **Location:** `tests/01_i2c_scanner/01_i2c_scanner.ino`
- **Purpose:** Verifies that your ESP32-S3 physically detects the MPU-6050 module over I2C bus.

---

## 🎯 Test 2: MPU-6050 Live Gyro & Steering Test (`02_mpu6050_test`)
- **Location:** `tests/02_mpu6050_test/02_mpu6050_test.ino`
- **Purpose:** Reads real-time rotation rate and calculates steering angle. Shows a visual steering bar graph in Serial Monitor.

---

## ⌨️ Test 3: USB HID Keyboard Simulation (`03_keyboard_test`)
- **Location:** `tests/03_keyboard_test/03_keyboard_test.ino`
- **Purpose:** Turns ESP32-S3 into a native USB Keyboard!
  - **Steer Left** (> 20° Left) ➔ Types **`A`** key
  - **Steer Right** (> 20° Right) ➔ Types **`D`** key
  - **Button GPIO 4** ➔ Types **`W`** key (Gas)
  - **Button GPIO 5** ➔ Types **`S`** key (Brake)
  - **Button GPIO 6** ➔ Types **`SPACEBAR`** (Handbrake)
- **Use Case:** Test steering directly in web browser games, arcade games, or Notepad!
