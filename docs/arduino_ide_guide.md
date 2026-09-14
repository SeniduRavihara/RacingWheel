ESP32-S3 Keyboard Working!

ESP32-S3 Keyboard Working!

ESP32-S3 Keyboard Working!

ESP32-S3 Keyboar dWorking!


# 📘 Complete ESP32-S3 Arduino IDE Upload & Configuration Guide

Detailed guide for setting up Arduino IDE, configuring menu settings, and uploading code to the **ESP32-S3 DevKitC-1 N16R8** board for the DIY Racing Wheel project.

---

## 📌 1. Prerequisites (Board Package Installation)

If you haven't installed the ESP32 core in Arduino IDE:

1. Open **Arduino IDE**.
2. Go to **File ➔ Preferences** (or `Ctrl + ,`).
3. In **Additional Boards Manager URLs**, paste:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**.
5. Go to **Tools ➔ Board ➔ Boards Manager...**
6. Search for **`esp32`** (by *Espressif Systems*).
7. Click **Install** (Version 2.0.x or 3.x).

---

## 🔌 2. Physical Ports on ESP32-S3 DevKitC-1

Your board has **TWO USB-C ports**:

```
 ┌──────────────────────────────────────────────┐
 │               ESP32-S3 BOARD                 │
 │                                              │
 │    [ USB ]                      [ UART ]     │
 └──────┬──────────────────────────────┬────────┘
        │                              │
  Native USB (OTG)            USB-to-Serial Bridge
  (For Gamepad/Keyboard)      (Primary for Uploading)
```

- **`UART` (or `COM`) Port:** Connected to the CH340 / CP2102 chip. Use this port to **upload code** and view standard `Serial.print()` messages.
- **`USB` (or `OTG`) Port:** Connected directly to ESP32-S3 GPIO 19/20. Use this port when running **USB HID Keyboard or Gamepad** simulation.

---

## ⚙️ 3. Arduino IDE `Tools` Menu Settings

Before uploading any sketch, configure the **Tools** menu in Arduino IDE as follows:

| Menu Item                 | Exact Option to Select                                     | Description                                               |
| ------------------------- | ---------------------------------------------------------- | --------------------------------------------------------- |
| **Board**           | **`ESP32S3 Dev Module`**                           | Target micro-controller family                            |
| **Flash Size**      | **`16MB (128Mb)`**                                 | Matches**N16** specs (16MB Flash)                   |
| **PSRAM**           | **`OPI PSRAM`**                                    | Matches**R8** specs (8MB Octal PSRAM)               |
| **USB Mode**        | **`USB-OTG (TinyUSB)`**                            | **CRITICAL:** Enables native USB Keyboard & Gamepad |
| **USB CDC On Boot** | **`Enabled`**                                      | Allows`Serial.print()` to work over USB                 |
| **Upload Mode**     | **`UART0 / Hardware CDC`**                         | Standard upload mechanism                                 |
| **CPU Frequency**   | **`240MHz (WiFi/BT)`**                             | High performance mode                                     |
| **Port**            | Select your**`COMx`** or **`/dev/tty...`** | Port assigned to your board                               |

---

## 🚀 4. Step-by-Step Upload Procedure

### Step A: Connect Board

Plug a data-capable USB-C cable into the **`UART` port** of your ESP32-S3 and connect it to your computer.

### Step B: Select Port

In Arduino IDE, go to **Tools ➔ Port** and select your board's COM port (e.g. `COM3`, `COM4`, or `/dev/ttyACM0`).

### Step C: Verify Code

Click the **Verify (Checkmark ✓)** button to compile.

### Step D: Upload Code

Click the **Upload (Right Arrow ➔)** button.

> 💡 **If Upload Fails / "A fatal error occurred: Failed to connect to ESP32-S3":**
>
> 1. Press and **HOLD** the **`BOOT` button** on the ESP32-S3 board.
> 2. Press and release the **`RESET` (RST)** button once while still holding `BOOT`.
> 3. Release the **`BOOT` button** (Board is now in Bootloader mode).
> 4. Click **Upload** again in Arduino IDE.

---

## 🎮 5. Post-Upload: Running HID Keyboard / Gamepad

Once uploading completes with `Done uploading`:

1. Unplug the USB cable from the **`UART` port**.
2. Plug the USB cable into the **`USB` (OTG) port** of the board.
3. Your computer will detect a new device called **"ESP32-S3 Keyboard"** or **"ESP32-S3 Gamepad"**!
4. Open **Notepad** or an online key tester to verify key presses!

---

## 🛠️ 6. Troubleshooting

| Problem                                         | Cause                                         | Solution                                                         |
| ----------------------------------------------- | --------------------------------------------- | ---------------------------------------------------------------- |
| `USBHIDKeyboard.h: No such file or directory` | Incorrect USB Mode selected                   | Set`Tools ➔ USB Mode` to **`USB-OTG (TinyUSB)`**.     |
| `No COM port appearing`                       | Charge-only USB cable or missing drivers      | Use a data USB cable; install CP210x or CH340 drivers if needed. |
| `Board doesn't type into Notepad`             | Cable plugged into`UART` instead of `USB` | Move USB cable to the**`USB` (Native OTG)** port.        |
| `Typing happens too fast / repeating`         | No delay in`loop()`                         | Add`delay(15);` at the end of your main loop.                  |
