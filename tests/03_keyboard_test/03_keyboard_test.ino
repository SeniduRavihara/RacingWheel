// ============================================================
//  🧪 TEST 3: ESP32-S3 USB HID Keyboard Simulation
//  Converts MPU-6050 Steering & Buttons into PC Keyboard inputs!
//
//  How it works:
//    - Turn Left  (Angle < -20°)  --> Sends 'A' key (Steer Left)
//    - Turn Right (Angle > +20°)  --> Sends 'D' key (Steer Right)
//    - Center     (-20° to +20°)  --> Releases keys
//    - GPIO 4 Button              --> Sends 'W' key (Throttle / Gas)
//    - GPIO 5 Button              --> Sends 'S' key (Brake / Reverse)
//    - GPIO 6 Button              --> Sends SPACEBAR (Handbrake)
//
//  Arduino IDE Board Settings (CRITICAL!):
//    - Board: "ESP32S3 Dev Module"
//    - USB Mode: "USB-OTG (TinyUSB)"
//    - USB CDC On Boot: "Disabled"  (or "Enabled" for debug)
// ============================================================

#include <Wire.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

// Hardware Pins
#define SDA_PIN        8
#define SCL_PIN        9
#define MPU_ADDR       0x68

#define BTN_GAS        4  // Push button to GPIO 4 (Gas/Throttle)
#define BTN_BRAKE      5  // Push button to GPIO 5 (Brake)
#define BTN_HANDBRAKE  6  // Push button to GPIO 6 (Handbrake)
#define STATUS_LED     2  // Onboard LED

// Steering Sensitivity Thresholds
#define THRESHOLD_LEFT   -20.0f  // Angle to trigger 'A' key
#define THRESHOLD_RIGHT   20.0f  // Angle to trigger 'D' key

// Objects
USBHIDKeyboard Keyboard;

// Sensor variables
float gyroZBias     = 0.0f;
float steeringAngle = 0.0f;
unsigned long lastTime = 0;

// Key Press State Tracker (prevents repeating key flood)
bool keyAPressed    = false;
bool keyDPressed    = false;
bool keyWPressed    = false;
bool keySPressed    = false;
bool keySpacePressed= false;

void setup() {
  Serial.begin(115200);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Setup buttons with internal pull-up (button connects to GND)
  pinMode(BTN_GAS, INPUT_PULLUP);
  pinMode(BTN_BRAKE, INPUT_PULLUP);
  pinMode(BTN_HANDBRAKE, INPUT_PULLUP);

  // Init Wire & MPU-6050
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  // Wake up MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Initialize USB HID Keyboard
  Keyboard.begin();
  USB.begin();
  delay(500); // Wait for USB enumeration

  // Gyro Calibration (2 seconds)
  digitalWrite(STATUS_LED, HIGH);
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
  digitalWrite(STATUS_LED, LOW);

  lastTime = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;

  // 1. Read MPU-6050
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

  // Complementary filter
  steeringAngle = 0.98f * (steeringAngle + gyroZ_deg * dt) + 0.02f * accelAngle;

  // 2. Handle Steering -> Keyboard ('A' & 'D')
  if (steeringAngle < THRESHOLD_LEFT) {
    if (!keyAPressed) {
      Keyboard.press('a'); // Press 'A' for Left
      keyAPressed = true;
    }
    if (keyDPressed) {
      Keyboard.release('d');
      keyDPressed = false;
    }
  } 
  else if (steeringAngle > THRESHOLD_RIGHT) {
    if (!keyDPressed) {
      Keyboard.press('d'); // Press 'D' for Right
      keyDPressed = true;
    }
    if (keyAPressed) {
      Keyboard.release('a');
      keyAPressed = false;
    }
  } 
  else {
    // Wheel centered -> Release left & right keys
    if (keyAPressed) { Keyboard.release('a'); keyAPressed = false; }
    if (keyDPressed) { Keyboard.release('d'); keyDPressed = false; }
  }

  // 3. Handle Buttons -> Keyboard ('W', 'S', Spacebar)
  bool gasPressed       = !digitalRead(BTN_GAS);       // Active LOW
  bool brakePressed     = !digitalRead(BTN_BRAKE);     // Active LOW
  bool handbrakePressed = !digitalRead(BTN_HANDBRAKE); // Active LOW

  // Gas ('W')
  if (gasPressed && !keyWPressed) {
    Keyboard.press('w');
    keyWPressed = true;
  } else if (!gasPressed && keyWPressed) {
    Keyboard.release('w');
    keyWPressed = false;
  }

  // Brake ('S')
  if (brakePressed && !keySPressed) {
    Keyboard.press('s');
    keySPressed = true;
  } else if (!brakePressed && keySPressed) {
    Keyboard.release('s');
    keySPressed = false;
  }

  // Handbrake (Spacebar)
  if (handbrakePressed && !keySpacePressed) {
    Keyboard.press(' ');
    keySpacePressed = true;
  } else if (!handbrakePressed && keySpacePressed) {
    Keyboard.release(' ');
    keySpacePressed = false;
  }

  delay(10); // Smooth 100Hz scanning rate
}
