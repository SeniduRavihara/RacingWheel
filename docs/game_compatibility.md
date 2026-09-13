# 🎮 Game Compatibility

## Setup in Games

The racing wheel appears as a standard **HID Gamepad / Joystick**.

### Windows
1. Press `Win + R` → type `joy.cpl` → Enter
2. You should see "ESP32-S3 Racing Wheel" or similar
3. Click Properties to test axes and buttons

### Linux
```bash
# Install jstest
sudo apt install joystick
# List devices
ls /dev/input/js*
# Test
jstest /dev/input/js0
```

## Game-Specific Settings

### Assetto Corsa
- Controls → Steering Wheel
- Select "Gamepad" → assign steering axis
- Set steering range to 540°
- Calibrate in-game

### BeamNG.drive
- Options → Controls → Add/Edit Device
- Assign axes automatically via detection

### F1 24
- Settings → Controls → Custom
- Map steering to Left Stick X

## Tested Status

| Game | Notes | Status |
|------|-------|--------|
| Assetto Corsa | Works perfectly | ✅ |
| BeamNG.drive | Works perfectly | ✅ |
| F1 24 | Works, needs in-game calibration | ✅ |
| Dirt Rally 2.0 | Works | ✅ |
| Euro Truck Simulator 2 | Works | ✅ |
| Gran Turismo 7 (PS5 BLE) | Under testing | 🔄 |
