// ============================================================
//  🧪 STANDALONE USB KEYBOARD TEST (NO GYRO REQUIRED!)
//  Use this sketch to test if your ESP32-S3 can act as a keyboard.
//
//  What it does:
//    1. Types "ESP32-S3 Keyboard Working! " automatically every 3 seconds!
//    2. GPIO 4 to GND  --> Types 'w' (Gas)
//    3. GPIO 5 to GND  --> Types 's' (Brake)
//    4. GPIO 6 to GND  --> Types 'a' (Steer Left)
//    5. GPIO 7 to GND  --> Types 'd' (Steer Right)
//
//  Arduino IDE Board Settings:
//    - Board: "ESP32S3 Dev Module"
//    - USB Mode: "USB-OTG (TinyUSB)"
// ============================================================

#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

#define BTN_W  4
#define BTN_S  5
#define BTN_A  6
#define BTN_D  7

unsigned long lastAutoType = 0;

void setup() {
  pinMode(BTN_W, INPUT_PULLUP);
  pinMode(BTN_S, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);

  // Initialize USB HID Keyboard
  Keyboard.begin();
  USB.begin();
  delay(1000);
}

void loop() {
  unsigned long now = millis();

  // --- 1. Auto-Type Test Message Every 3 Seconds ---
  if (now - lastAutoType >= 3000) {
    lastAutoType = now;
    Keyboard.println("ESP32-S3 Keyboard Working!");
  }

  // --- 2. Test Pin Shorting (Touch GPIO pin to GND) ---
  if (!digitalRead(BTN_W)) { Keyboard.press('w'); } else { Keyboard.release('w'); }
  if (!digitalRead(BTN_S)) { Keyboard.press('s'); } else { Keyboard.release('s'); }
  if (!digitalRead(BTN_A)) { Keyboard.press('a'); } else { Keyboard.release('a'); }
  if (!digitalRead(BTN_D)) { Keyboard.press('d'); } else { Keyboard.release('d'); }

  delay(20);
}
