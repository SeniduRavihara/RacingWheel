#pragma once
#include <Arduino.h>

// ============================================================
//  UsbHidGamepad.h
//  ESP32-S3 USB HID Gamepad (plug & play — no drivers needed)
//  Uses Arduino USB HID library (built into ESP32-S3 Arduino core)
// ============================================================

class UsbHidGamepad {
public:
    UsbHidGamepad();
    void begin();

    // steeringAngle: –540.0 to +540.0 degrees
    // buttons: bitmask of button states
    void sendReport(float steeringAngle, uint16_t buttons);
};
