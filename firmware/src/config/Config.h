#pragma once

// ============================================================
//  RacingWheel - Config.h
//  All hardware pin definitions and tuning parameters
//  Edit this file to match your physical wiring
// ============================================================

// --- I2C Pins (MPU-6050) ---
#define I2C_SDA_PIN       8
#define I2C_SCL_PIN       9
#define MPU6050_ADDRESS   0x68   // AD0 → GND = 0x68 | AD0 → 3.3V = 0x69

// --- Button GPIO Pins ---
#define BTN_GEAR_UP       4
#define BTN_GEAR_DOWN     5
#define BTN_A             6
#define BTN_B             7
#define BTN_X             15
#define BTN_Y             16

// --- LED ---
#define STATUS_LED_PIN    2

// --- Steering Tuning ---
#define STEERING_MAX_DEG        540.0f   // Max wheel rotation in degrees (±540°)
#define GYRO_DRIFT_THRESHOLD    0.05f    // rad/s — below this = "stationary"
#define DRIFT_CORRECTION        0.995f   // How fast to re-center (0.99 = faster, 0.999 = slower)
#define COMPLEMENTARY_ALPHA     0.98f    // Gyro weight in complementary filter (0–1)

// --- USB HID ---
#define HID_REPORT_INTERVAL_MS  8        // Send HID report every 8ms (~125Hz)

// --- Connection Mode ---
// Comment/uncomment to select connection type:
#define USE_USB_HID
// #define USE_BLE

// --- Debug ---
// Uncomment to enable Serial debug output:
// #define DEBUG_SERIAL
