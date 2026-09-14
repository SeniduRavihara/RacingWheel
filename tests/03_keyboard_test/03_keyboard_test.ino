// ============================================================
//  🧪 TEST 3: ESP32-S3 USB HID Keyboard Simulation
//  Works WITH or WITHOUT MPU-6050 Gyro Connected!
//
//  How it works:
//    1. Auto-detects MPU-6050 gyro:
//       - If Gyro present: Tilting Left/Right presses 'A' / 'D' keys.
//       - If Gyro NOT present: Automatically types test message every 3s.
//    2. GPIO Pin Button Inputs (Active LOW - short to GND):
//       - GPIO 4 --> Presses 'W' key (Throttle)
//       - GPIO 5 --> Presses 'S' key (Brake)
//       - GPIO 6 --> Presses 'A' key (Steer Left / Button)
//       - GPIO 7 --> Presses 'D' key (Steer Right / Button)
//
//  Arduino IDE Board Settings (CRITICAL!):
//    - Board: "ESP32S3 Dev Module"
//    - USB Mode: "USB-OTG (TinyUSB)"
// ============================================================

#include <Wire.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

// Pin Definitions
#define SDA_PIN        8
#define SCL_PIN        9
#define MPU_ADDR       0x68

#define BTN_W          4  // Short GPIO 4 to GND -> 'W' key
#define BTN_S          5  // Short GPIO 5 to GND -> 'S' key
#define BTN_A          6  // Short GPIO 6 to GND -> 'A' key
#define BTN_D          7  // Short GPIO 7 to GND -> 'D' key

// Objects
USBHIDKeyboard Keyboard;

// State Variables
bool mpuPresent     = false;
float gyroZBias     = 0.0f;
float steeringAngle = 0.0f;
unsigned long lastTime     = 0;
unsigned long lastAutoType = 0;

// Key Press State Tracker
bool keyAPressed = false;
bool keyDPressed = false;
bool keyWPressed = false;
bool keySPressed = false;

void setup() {
  Serial.begin(115200);

  // Configure Button Pins with Pull-Up
  pinMode(BTN_W, INPUT_PULLUP);
  pinMode(BTN_S, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);

  // Initialize USB HID Keyboard
  Keyboard.begin();
  USB.begin();
  delay(1000);

  // Initialize I2C Bus
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  // Check if MPU-6050 is connected
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Power management
  Wire.write(0x00); // Wake up
  if (Wire.endTransmission() == 0) {
    mpuPresent = true;
    Serial.println("✅ MPU-6050 Gyro Detected!");

    // Calibrate Gyro (2 seconds)
    float sumZ = 0;
    for (int i = 0; i < 500; i++) {
      Wire.beginTransmission(MPU_ADDR);
      Wire.write(0x47);
      Wire.endTransmission(false);
      Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
      int16_t rawZ = (Wire.read() << 8) | Wire.read();
      sumZ += (rawZ / 131.0f);
      delay(4);
    }
    gyroZBias = sumZ / 500.0f;
  } else {
    mpuPresent = false;
    Serial.println("⚠️ No MPU-6050 detected. Running in Standalone Keyboard Mode!");
  }

  lastTime = millis();
}

void loop() {
  unsigned long now = millis();

  // --- MODE A: Gyro Control (If MPU-6050 is connected) ---
  if (mpuPresent) {
    float dt = (now - lastTime) / 1000.0f;
    lastTime = now;

    // Read MPU-6050 Accel & Gyro
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14);

    int16_t rawAX = (Wire.read() << 8) | Wire.read();
    Wire.read(); Wire.read(); // AY
    int16_t rawAZ = (Wire.read() << 8) | Wire.read();
    Wire.read(); Wire.read(); // Temp
    Wire.read(); Wire.read(); // GX
    Wire.read(); Wire.read(); // GY
    int16_t rawGZ = (Wire.read() << 8) | Wire.read();

    float ax = rawAX / 16384.0f;
    float az = rawAZ / 16384.0f;
    float gyroZ_deg = (rawGZ / 131.0f) - gyroZBias;
    float accelAngle = atan2f(ax, az) * 57.29578f;

    // Complementary filter for smooth steering angle
    steeringAngle = 0.98f * (steeringAngle + gyroZ_deg * dt) + 0.02f * accelAngle;

    // Steer Left ('A')
    if (steeringAngle < -20.0f) {
      if (!keyAPressed) { Keyboard.press('a'); keyAPressed = true; }
      if (keyDPressed)  { Keyboard.release('d'); keyDPressed = false; }
    } 
    // Steer Right ('D')
    else if (steeringAngle > 20.0f) {
      if (!keyDPressed) { Keyboard.press('d'); keyDPressed = true; }
      if (keyAPressed)  { Keyboard.release('a'); keyAPressed = false; }
    } 
    // Center
    else {
      if (keyAPressed) { Keyboard.release('a'); keyAPressed = false; }
      if (keyDPressed) { Keyboard.release('d'); keyDPressed = false; }
    }
  } 
  // --- MODE B: Standalone Auto-Type (If Gyro NOT connected) ---
  else {
    if (now - lastAutoType >= 3000) {
      lastAutoType = now;
      Keyboard.println("ESP32-S3 Keyboard Working!");
    }
  }

  // --- BUTTON INPUTS (Works in both modes) ---
  bool wPressed = !digitalRead(BTN_W);
  bool sPressed = !digitalRead(BTN_S);
  bool aPressed = !digitalRead(BTN_A);
  bool dPressed = !digitalRead(BTN_D);

  // 'W' key
  if (wPressed && !keyWPressed) { Keyboard.press('w'); keyWPressed = true; }
  else if (!wPressed && keyWPressed) { Keyboard.release('w'); keyWPressed = false; }

  // 'S' key
  if (sPressed && !keySPressed) { Keyboard.press('s'); keySPressed = true; }
  else if (!sPressed && keySPressed) { Keyboard.release('s'); keySPressed = false; }

  // 'A' key (Override if pin grounded)
  if (aPressed && !keyAPressed) { Keyboard.press('a'); keyAPressed = true; }
  else if (!aPressed && !mpuPresent && keyAPressed) { Keyboard.release('a'); keyAPressed = false; }

  // 'D' key (Override if pin grounded)
  if (dPressed && !keyDPressed) { Keyboard.press('d'); keyDPressed = true; }
  else if (!dPressed && !mpuPresent && keyDPressed) { Keyboard.release('d'); keyDPressed = false; }

  delay(15);
}
