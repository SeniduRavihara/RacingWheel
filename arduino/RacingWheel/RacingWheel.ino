// ============================================================
//  🏎️ ESP32-S3 DIY Racing Wheel
//  Arduino IDE Sketch
//
//  Hardware:
//    - ESP32-S3 DevKitC-1 N16R8
//    - MPU-6050 Gyro / Accelerometer (I2C)
//    - Push Buttons (gear shift + face buttons)
//
//  Libraries needed (install via Arduino Library Manager):
//    - "Adafruit MPU6050" by Adafruit
//    - "Adafruit Unified Sensor" by Adafruit
//    - "ESP32-BLE-Gamepad" by lemmingDev  (if using BLE mode)
//
//  Board settings in Arduino IDE:
//    - Board: "ESP32S3 Dev Module"
//    - USB Mode: "USB-OTG (TinyUSB)"
//    - USB CDC On Boot: "Disabled"
//    - Flash Size: "16MB"
//    - PSRAM: "OPI PSRAM"
// ============================================================

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "USB.h"
#include "USBHIDGamepad.h"

// ─────────────────────────────────────────────
//  PIN CONFIGURATION — Edit to match your wiring
// ─────────────────────────────────────────────
#define I2C_SDA             8     // MPU-6050 SDA
#define I2C_SCL             9     // MPU-6050 SCL

#define BTN_GEAR_UP         4     // Gear Up paddle
#define BTN_GEAR_DOWN       5     // Gear Down paddle
#define BTN_A               6     // Face button A
#define BTN_B               7     // Face button B
#define BTN_X               15    // Face button X
#define BTN_Y               16    // Face button Y

#define STATUS_LED          2     // Onboard / status LED

// ─────────────────────────────────────────────
//  TUNING PARAMETERS
// ─────────────────────────────────────────────
#define STEERING_MAX_DEG    540.0f   // Max wheel rotation (±540°)
#define DRIFT_THRESHOLD     0.05f    // rad/s — below this = "wheel is still"
#define DRIFT_CORRECTION    0.995f   // Re-center speed (lower = faster)
#define COMP_ALPHA          0.98f    // Complementary filter: gyro weight (0–1)
#define HID_INTERVAL_MS     8        // HID report rate (~125Hz)
#define CALIB_SAMPLES       500      // Samples taken during gyro calibration

// ─────────────────────────────────────────────
//  OBJECTS
// ─────────────────────────────────────────────
Adafruit_MPU6050  mpu;
USBHIDGamepad     gamepad;

// ─────────────────────────────────────────────
//  GLOBAL STATE
// ─────────────────────────────────────────────
float         steeringAngle  = 0.0f;   // Current wheel angle (degrees)
float         gyroZBias      = 0.0f;   // Gyro zero-rate bias
unsigned long lastTime       = 0;
unsigned long lastHIDSend    = 0;

// Button pins array for easy iteration
const uint8_t BTN_PINS[]    = { BTN_GEAR_UP, BTN_GEAR_DOWN, BTN_A, BTN_B, BTN_X, BTN_Y };
const uint8_t BTN_COUNT     = sizeof(BTN_PINS) / sizeof(BTN_PINS[0]);

// Debounce state
bool     btnState[6]        = {false};
bool     btnLastState[6]    = {false};
unsigned long btnDebounce[6] = {0};
#define DEBOUNCE_MS 20

// ─────────────────────────────────────────────
//  FUNCTION: Blink LED (error indicator)
// ─────────────────────────────────────────────
void blinkError() {
  while (true) {
    digitalWrite(STATUS_LED, HIGH); delay(100);
    digitalWrite(STATUS_LED, LOW);  delay(100);
  }
}

// ─────────────────────────────────────────────
//  FUNCTION: Calibrate gyro bias
//  Keep wheel STILL during this!
// ─────────────────────────────────────────────
void calibrateGyro() {
  Serial.println("Calibrating gyro — keep wheel STILL...");
  digitalWrite(STATUS_LED, HIGH);

  double biasSum = 0.0;
  for (int i = 0; i < CALIB_SAMPLES; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    biasSum += g.gyro.z;  // Z-axis = yaw (steering)
    delay(2);
  }
  gyroZBias = biasSum / CALIB_SAMPLES;

  Serial.print("Gyro Z bias: ");
  Serial.println(gyroZBias, 6);
  Serial.println("Calibration done! Ready to race 🏎️");

  digitalWrite(STATUS_LED, LOW);
  delay(200);
  digitalWrite(STATUS_LED, HIGH); // Solid ON = ready
}

// ─────────────────────────────────────────────
//  FUNCTION: Read + debounce all buttons
//  Returns bitmask of pressed buttons
// ─────────────────────────────────────────────
uint16_t readButtons() {
  unsigned long now = millis();
  uint16_t mask = 0;

  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    // Active LOW because INPUT_PULLUP
    bool reading = !digitalRead(BTN_PINS[i]);

    if (reading != btnLastState[i]) {
      btnDebounce[i] = now;
    }
    if ((now - btnDebounce[i]) > DEBOUNCE_MS) {
      btnState[i] = reading;
    }
    btnLastState[i] = reading;

    if (btnState[i]) mask |= (1 << i);
  }
  return mask;
}

// ─────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // Status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Buttons — INPUT_PULLUP means: wire button between GPIO and GND
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }

  // I2C + MPU-6050 init
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // 400kHz fast mode

  if (!mpu.begin()) {
    Serial.println("ERROR: MPU-6050 not found! Check wiring.");
    blinkError(); // Halts here
  }

  // Configure MPU-6050 gyro range: ±250°/s (most sensitive)
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  // Configure accelerometer: ±2g
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  // Low-pass filter: 21Hz (smooth out noise)
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU-6050 found! ✅");

  // USB HID Gamepad init
  gamepad.begin();
  USB.begin();
  delay(500); // Allow USB enumeration

  // Calibrate gyro
  calibrateGyro();

  lastTime = millis();
  Serial.println("=== Racing Wheel Active ===");
}

// ─────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────
void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;  // Delta time in seconds
  lastTime = now;

  // ── 1. Read gyro + accelerometer ──
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Remove calibration bias from Z-axis (yaw = steering)
  float gyroZ = g.gyro.z - gyroZBias;  // rad/s

  // ── 2. Complementary filter ──
  //    Blends gyro integration (fast, drifts) with
  //    accelerometer angle (slow, stable) to reduce drift
  float accelAngle = atan2f(a.acceleration.x, a.acceleration.z) * 57.29578f;

  steeringAngle = COMP_ALPHA * (steeringAngle + gyroZ * dt * 57.29578f)
                + (1.0f - COMP_ALPHA) * accelAngle;

  // ── 3. Anti-drift: gently re-center when wheel is still ──
  if (fabsf(gyroZ) < DRIFT_THRESHOLD) {
    steeringAngle *= DRIFT_CORRECTION;
  }

  // ── 4. Clamp to max rotation ──
  steeringAngle = constrain(steeringAngle, -STEERING_MAX_DEG, STEERING_MAX_DEG);

  // ── 5. Read buttons ──
  uint16_t buttons = readButtons();

  // ── 6. Send HID report at fixed rate ──
  if (now - lastHIDSend >= HID_INTERVAL_MS) {
    lastHIDSend = now;

    // Map steering angle (-540 to +540) → HID axis (-32767 to +32767)
    int16_t axisX = (int16_t)((steeringAngle / STEERING_MAX_DEG) * 32767.0f);
    axisX = constrain(axisX, -32767, 32767);

    gamepad.leftStick(axisX, 0);   // X axis = steering, Y = 0
    gamepad.buttons(buttons);       // Send button states
    gamepad.send();                 // Transmit HID report

    // Debug output (remove in production for speed)
    Serial.printf("Angle: %6.1f°  GyroZ: %5.3f  Buttons: 0b%06b\n",
                  steeringAngle, gyroZ, buttons);
  }
}
