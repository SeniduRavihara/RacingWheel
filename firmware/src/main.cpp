// ============================================================
//  RacingWheel - main.cpp
//  ESP32-S3 DIY Racing Wheel Firmware
//
//  Hardware:
//    - ESP32-S3 DevKitC-1 N16R8
//    - MPU-6050 Gyro/Accelerometer (I2C)
//    - Push buttons (gear shift, face buttons)
//
//  Connection:
//    - USB HID Gamepad (plug & play)
//
//  Author: YourName
//  License: MIT
// ============================================================

#include <Arduino.h>
#include "config/Config.h"
#include "gyro/GyroSensor.h"
#include "buttons/ButtonMatrix.h"

#ifdef USE_USB_HID
  #include "hid/UsbHidGamepad.h"
  UsbHidGamepad gamepad;
#endif

GyroSensor     gyro;
ButtonMatrix   buttons;

unsigned long lastHidSend = 0;

void setup() {
#ifdef DEBUG_SERIAL
    Serial.begin(115200);
    Serial.println("=== Racing Wheel Starting ===");
#endif

    // --- Status LED ---
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    // --- Buttons ---
    buttons.addButton(BTN_GEAR_UP);
    buttons.addButton(BTN_GEAR_DOWN);
    buttons.addButton(BTN_A);
    buttons.addButton(BTN_B);
    buttons.addButton(BTN_X);
    buttons.addButton(BTN_Y);
    buttons.begin();

    // --- USB HID ---
#ifdef USE_USB_HID
    gamepad.begin();
    delay(500); // Allow USB enumeration
#endif

    // --- Gyro Init ---
    if (!gyro.begin()) {
#ifdef DEBUG_SERIAL
        Serial.println("ERROR: MPU-6050 not found! Check wiring.");
#endif
        // Blink LED rapidly to indicate error
        while (true) {
            digitalWrite(STATUS_LED_PIN, HIGH); delay(100);
            digitalWrite(STATUS_LED_PIN, LOW);  delay(100);
        }
    }

#ifdef DEBUG_SERIAL
    Serial.println("Calibrating gyro — keep wheel STILL for 1 second...");
#endif

    // Flash LED during calibration
    digitalWrite(STATUS_LED_PIN, HIGH);
    gyro.calibrate(500);
    digitalWrite(STATUS_LED_PIN, LOW);

#ifdef DEBUG_SERIAL
    Serial.println("Calibration done! Ready to race 🏎️");
#endif

    // Solid LED = ready
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void loop() {
    // --- Update sensors ---
    gyro.update();
    buttons.update();

    // --- Send HID report at fixed interval ---
    unsigned long now = millis();
    if (now - lastHidSend >= HID_REPORT_INTERVAL_MS) {
        lastHidSend = now;

        float steeringAngle = gyro.getSteeringAngle();
        uint16_t buttonMask = buttons.getBitmask();

#ifdef USE_USB_HID
        gamepad.sendReport(steeringAngle, buttonMask);
#endif

#ifdef DEBUG_SERIAL
        Serial.printf("Angle: %6.1f°  Buttons: 0x%04X\n", steeringAngle, buttonMask);
#endif
    }
}
