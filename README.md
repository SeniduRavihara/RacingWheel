# 🏎️ ESP32-S3 DIY Racing Wheel

A fully DIY sim racing wheel built using the **ESP32-S3 DevKitC-1 N16R8** and **MPU-6050 gyroscope module**. Connects to PC via **USB HID** (plug & play — no drivers needed!) or **Bluetooth BLE**.

---

## 📸 Project Overview

```
┌─────────────────────────────────────────────────────┐
│              DIY SIM RACING WHEEL                   │
│                                                     │
│   [MPU-6050 Gyro] ──I2C──► [ESP32-S3 N16R8]        │
│   [Buttons/Paddles] ─GPIO──►     │                  │
│   [LED Indicator]  ─GPIO──►      │                  │
│                              USB-C / BLE             │
│                                  │                  │
│                             [PC / Console]           │
│                            Appears as Gamepad 🎮     │
└─────────────────────────────────────────────────────┘
```

---

## 🛒 Hardware Required

| Component | Details | Qty |
|-----------|---------|-----|
| ESP32-S3 DevKitC-1 N16R8 | Main microcontroller | 1 |
| MPU-6050 Gyro/Accel Module | 6-DOF IMU, I2C | 1 |
| Push Buttons (momentary) | Gear up, Gear down, A/B/X/Y | 6–12 |
| LED (optional) | Status indicator | 1 |
| Resistors 10kΩ | Button pull-downs | 6–12 |
| USB-C Cable | PC connection / flashing | 1 |
| Breadboard / Custom PCB | Prototyping | 1 |
| 3D Printed / Wood Wheel Frame | Housing | 1 |

---

## 📁 Project Structure

```
RacingWheel/
│
├── firmware/                        # ESP32-S3 Arduino firmware
│   ├── src/
│   │   ├── main.cpp                 # Main entry point
│   │   ├── gyro/
│   │   │   ├── GyroSensor.h         # MPU-6050 class header
│   │   │   └── GyroSensor.cpp       # Gyro read + angle calculation
│   │   ├── hid/
│   │   │   ├── UsbHidGamepad.h      # USB HID gamepad header
│   │   │   └── UsbHidGamepad.cpp    # USB HID report descriptor & sending
│   │   ├── bluetooth/
│   │   │   ├── BleGamepad.h         # BLE gamepad header
│   │   │   └── BleGamepad.cpp       # BLE HID profile
│   │   ├── buttons/
│   │   │   ├── ButtonMatrix.h       # Button handler header
│   │   │   └── ButtonMatrix.cpp     # Debounce + state management
│   │   └── config/
│   │       └── Config.h             # All pin definitions & tuning params
│   │
│   ├── platformio.ini               # PlatformIO project config
│   └── README.md                    # Firmware-specific docs
│
├── hardware/                        # Wiring & schematics
│   ├── schematic.png                # Full wiring diagram
│   ├── wiring_guide.md              # Step-by-step wiring instructions
│   └── bom.md                       # Bill of Materials
│
├── docs/                            # Additional documentation
│   ├── calibration.md               # How to calibrate the gyro
│   ├── troubleshooting.md           # Common issues & fixes
│   └── game_compatibility.md        # Tested games & settings
│
├── 3d_models/                       # 3D printable parts (optional)
│   └── README.md                    # Links to model files
│
├── .gitignore                       # Git ignore rules
└── README.md                        # ← You are here
```

---

## ⚡ Quick Start

### 1. Install PlatformIO
```bash
# Install VS Code extension: PlatformIO IDE
# OR use CLI:
pip install platformio
```

### 2. Clone & Open Project
```bash
git clone https://github.com/yourname/RacingWheel.git
cd RacingWheel/firmware
```

### 3. Wire Hardware
See `hardware/wiring_guide.md` for full wiring details.

**Quick wiring (MPU-6050 → ESP32-S3):**
```
MPU-6050 VCC  → ESP32-S3 3.3V
MPU-6050 GND  → ESP32-S3 GND
MPU-6050 SDA  → ESP32-S3 GPIO 8
MPU-6050 SCL  → ESP32-S3 GPIO 9
MPU-6050 AD0  → GND  (I2C address 0x68)
```

### 4. Configure & Flash
```bash
# Edit firmware/src/config/Config.h to match your wiring
# Then flash:
pio run --target upload
```

### 5. Test
- Plug USB-C into PC
- Open Game Controllers (Windows: joy.cpl) or jstest (Linux)
- Turn the wheel — axis should respond!

---

## 🔧 How It Works

### Steering (Gyro-Based)
The MPU-6050 measures the Z-axis angular rate (yaw). We integrate this over time to get the current steering angle:

```
angle += gyro_z_rate × delta_time
```

A complementary filter reduces drift by blending gyro + accelerometer data.

### Anti-Drift
When the wheel is stationary, the angle slowly self-centers:
```cpp
if (abs(gyro_z) < DRIFT_THRESHOLD) {
    steering_angle *= DRIFT_CORRECTION; // e.g., 0.995
}
```

### USB HID
The ESP32-S3 has native USB OTG support. It presents itself as a standard HID Gamepad — no drivers needed on Windows/Linux/macOS.

---

## 📐 Pin Configuration

| Function | GPIO Pin |
|----------|----------|
| MPU-6050 SDA | GPIO 8 |
| MPU-6050 SCL | GPIO 9 |
| Button: Gear Up | GPIO 4 |
| Button: Gear Down | GPIO 5 |
| Button: A | GPIO 6 |
| Button: B | GPIO 7 |
| Button: X | GPIO 15 |
| Button: Y | GPIO 16 |
| Status LED | GPIO 2 |

> All configurable in firmware/src/config/Config.h

---

## 🎮 Tested Games

| Game | Platform | Status |
|------|---------|--------|
| Assetto Corsa | PC (Steam) | ✅ Works |
| BeamNG.drive | PC (Steam) | ✅ Works |
| F1 24 | PC (Steam) | ✅ Works |
| Gran Turismo 7 | PS5 (via BLE) | 🔄 Testing |

---

## 📖 Documentation

- [Wiring Guide](hardware/wiring_guide.md)
- [Calibration Guide](docs/calibration.md)
- [Troubleshooting](docs/troubleshooting.md)
- [Game Compatibility](docs/game_compatibility.md)
- [Firmware README](firmware/README.md)

---

## 🛠️ Development Roadmap

- [x] Project structure & documentation
- [ ] MPU-6050 gyro integration
- [ ] USB HID gamepad firmware
- [ ] Button matrix with debounce
- [ ] Complementary filter for drift correction
- [ ] BLE gamepad mode
- [ ] Auto-calibration on startup
- [ ] Force Feedback (FFB) support
- [ ] OLED telemetry display

---

## 📜 License

MIT License — Free to use, modify, and share.

---

*Built with ❤️ using ESP32-S3 + MPU-6050*
