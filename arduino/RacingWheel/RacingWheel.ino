// ============================================================
//  🏎️ ESP32-S3 DIY Racing Wheel Firmware (Gamepad Mode)
//  Compatible with ESP32 Arduino Core 3.x & 2.x
//
//  Hardware:
//    - ESP32-S3 DevKitC-1 N16R8
//    - MPU-6050 Gyro/Accelerometer (I2C: SDA=8, SCL=9)
//    - Push Buttons for Shifters & Actions (GPIO 4, 5, 6, 7, 15, 16)
//
//  Arduino IDE Board Settings:
//    - Board: "ESP32S3 Dev Module"
//    - USB Mode: "USB-OTG (TinyUSB)"
// ============================================================

#include <Wire.h>
#include "USB.h"
#include "USBHIDGamepad.h"

// ─────────────────────────────────────────────
//  PIN CONFIGURATION
// ─────────────────────────────────────────────
#define SDA_PIN             8     // MPU-6050 SDA
#define SCL_PIN             9     // MPU-6050 SCL
#define MPU_ADDR            0x68  // MPU-6050 I2C Address

#define BTN_GEAR_UP         4     // Paddle Shift Up   -> Gamepad Button 0 (B0)
#define BTN_GEAR_DOWN       5     // Paddle Shift Down -> Gamepad Button 1 (B1)
#define BTN_A               6     // Button A          -> Gamepad Button 2 (B2)
#define BTN_B               7     // Button B          -> Gamepad Button 3 (B3)
#define BTN_X               15    // Button X          -> Gamepad Button 4 (B4)
#define BTN_Y               16    // Button Y          -> Gamepad Button 5 (B5)
#define STATUS_LED          2     // Onboard LED

// ─────────────────────────────────────────────
//  TUNING PARAMETERS
// ─────────────────────────────────────────────
#define STEERING_MAX_DEG    90.0f    // Max gyro tilt angle for 100% axis range (±90°)
#define DRIFT_THRESHOLD     0.05f    // rad/s — threshold for stationary wheel
#define DRIFT_CORRECTION    0.995f   // Self-centering rate (0.99 = faster)
#define COMP_ALPHA          0.98f    // Complementary filter weight (0–1)
#define HID_INTERVAL_MS     10       // HID report rate (100Hz)
#define CALIB_SAMPLES       500      // Startup gyro calibration samples

// ─────────────────────────────────────────────
//  OBJECTS & GLOBALS
// ─────────────────────────────────────────────
USBHIDGamepad gamepad;

float steeringAngle = 0.0f;
float gyroZBias     = 0.0f;
unsigned long lastTime    = 0;
unsigned long lastHIDSend = 0;

// Button pins array
const uint8_t BTN_PINS[] = { BTN_GEAR_UP, BTN_GEAR_DOWN, BTN_A, BTN_B, BTN_X, BTN_Y };
const uint8_t BTN_COUNT  = sizeof(BTN_PINS) / sizeof(BTN_PINS[0]);

bool btnState[6]         = {false};
bool btnLastState[6]     = {false};
unsigned long btnDebounce[6] = {0};
#define DEBOUNCE_MS 20

// ─────────────────────────────────────────────
//  Read Buttons with Debounce
// ─────────────────────────────────────────────
uint32_t readButtons() {
  unsigned long now = millis();
  uint32_t mask = 0;

  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    bool reading = !digitalRead(BTN_PINS[i]); // Active LOW

    if (reading != btnLastState[i]) {
      btnDebounce[i] = now;
    }
    if ((now - btnDebounce[i]) > DEBOUNCE_MS) {
      btnState[i] = reading;
    }
    btnLastState[i] = reading;

    if (btnState[i]) mask |= (1UL << i);
  }
  return mask;
}

// ─────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Setup buttons with internal pullup
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }

  // Init Wire & MPU-6050
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  // Wake up MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Power Management 1
  Wire.write(0x00);
  Wire.endTransmission();

  // Set Gyro sensitivity range ±250 deg/s
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Init USB HID Gamepad
  gamepad.begin();
  USB.begin();
  delay(500);

  // Calibrate Gyro (2 seconds stillness)
  digitalWrite(STATUS_LED, HIGH);
  float sumZ = 0;
  for (int i = 0; i < CALIB_SAMPLES; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47); // GYRO_ZOUT_H register
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
    int16_t rawZ = (Wire.read() << 8) | Wire.read();
    sumZ += (rawZ / 131.0f);
    delay(4);
  }
  gyroZBias = sumZ / CALIB_SAMPLES;

  digitalWrite(STATUS_LED, LOW);
  delay(100);
  digitalWrite(STATUS_LED, HIGH); // Solid ON = Ready

  lastTime = millis();
}

// ─────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────
void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;

  // 1. Read Gyro Z-axis directly from 0x47
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47); // GYRO_ZOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
  int16_t rawGZ = (Wire.read() << 8) | Wire.read();
  float gyroZ = (rawGZ / 131.0f) - gyroZBias; // deg/s

  // 2. Read Accel X & Z for reference angle from 0x3B
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6);
  int16_t rawAX = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // AY
  int16_t rawAZ = (Wire.read() << 8) | Wire.read();

  float ax = rawAX / 16384.0f;
  float az = rawAZ / 16384.0f;
  float accelAngle = atan2f(ax, az) * 57.29578f;

  // 3. Complementary Filter for Smooth Angle
  steeringAngle = COMP_ALPHA * (steeringAngle + gyroZ * dt) + (1.0f - COMP_ALPHA) * accelAngle;

  // 4. Anti-drift gentle re-centering when stationary
  if (fabsf(gyroZ) < DRIFT_THRESHOLD) {
    steeringAngle *= DRIFT_CORRECTION;
  }

  // 5. Clamp to maximum steering rotation (±90°)
  steeringAngle = constrain(steeringAngle, -STEERING_MAX_DEG, STEERING_MAX_DEG);

  // 6. Read Button States
  uint32_t buttons = readButtons();

  // 7. Send HID Gamepad Report (100Hz)
  if (now - lastHIDSend >= HID_INTERVAL_MS) {
    lastHIDSend = now;

    // Map steering angle (-90° to +90°) -> Gamepad X Axis (-127 to +127)
    int8_t steerAxis = (int8_t)constrain(map((long)(steeringAngle * 10.0f), -900, 900, -127, 127), -127, 127);

    // Signature: send(x, y, z, rz, rx, ry, hat, buttons)
    gamepad.send(steerAxis, 0, 0, 0, 0, 0, 0, buttons);
  }
}
