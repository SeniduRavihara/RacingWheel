#include "GyroSensor.h"
#include "../config/Config.h"

// MPU-6050 register addresses
#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_GYRO_CONFIG  0x1B
#define MPU6050_REG_ACCEL_XOUT   0x3B
#define MPU6050_REG_GYRO_XOUT    0x43

// Scale factors
#define GYRO_SCALE_250DPS   131.0f   // LSB per °/s at ±250°/s range
#define ACCEL_SCALE_2G      16384.0f // LSB per g at ±2g range
#define DEG_TO_RAD          0.017453f
#define RAD_TO_DEG          57.29578f

GyroSensor::GyroSensor()
    : _steeringAngle(0), _gyroZBias(0), _gyroZ(0),
      _accAngle(0), _lastTime(0), _calibrated(false) {}

bool GyroSensor::begin() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000); // 400kHz fast mode

    // Wake up MPU-6050 (it starts in sleep mode)
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_PWR_MGMT_1);
    Wire.write(0x00); // Clear sleep bit
    if (Wire.endTransmission() != 0) return false;

    // Set gyro range to ±250°/s (most sensitive)
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_GYRO_CONFIG);
    Wire.write(0x00);
    Wire.endTransmission();

    _lastTime = millis();
    return true;
}

void GyroSensor::calibrate(int samples) {
    float biasSum = 0;
    for (int i = 0; i < samples; i++) {
        float ax, ay, az, gx, gy, gz;
        readRaw(ax, ay, az, gx, gy, gz);
        biasSum += gz; // Z-axis for steering (yaw)
        delay(2);
    }
    _gyroZBias = biasSum / samples;
    _calibrated = true;
}

void GyroSensor::update() {
    unsigned long now = millis();
    float dt = (now - _lastTime) / 1000.0f; // seconds
    _lastTime = now;

    float ax, ay, az, gx, gy, gz;
    readRaw(ax, ay, az, gx, gy, gz);

    // Remove bias and convert to °/s
    _gyroZ = (gz - _gyroZBias) / GYRO_SCALE_250DPS;

    // Complementary filter — blend gyro integration with accel angle
    float accelAngle = atan2f(ax, az) * RAD_TO_DEG;
    _steeringAngle = COMPLEMENTARY_ALPHA * (_steeringAngle + _gyroZ * dt)
                   + (1.0f - COMPLEMENTARY_ALPHA) * accelAngle;

    // Anti-drift: gently re-center when stationary
    if (fabsf(_gyroZ) < GYRO_DRIFT_THRESHOLD) {
        _steeringAngle *= DRIFT_CORRECTION;
    }

    // Clamp to max rotation
    _steeringAngle = constrain(_steeringAngle, -STEERING_MAX_DEG, STEERING_MAX_DEG);
}

float GyroSensor::getSteeringAngle() const { return _steeringAngle; }
float GyroSensor::getGyroZ() const         { return _gyroZ; }
void  GyroSensor::resetAngle()              { _steeringAngle = 0; }

void GyroSensor::readRaw(float &ax, float &ay, float &az,
                          float &gx, float &gy, float &gz) {
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_ACCEL_XOUT);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU6050_ADDRESS, (uint8_t)14);

    ax = (int16_t)((Wire.read() << 8) | Wire.read()) / ACCEL_SCALE_2G;
    ay = (int16_t)((Wire.read() << 8) | Wire.read()) / ACCEL_SCALE_2G;
    az = (int16_t)((Wire.read() << 8) | Wire.read()) / ACCEL_SCALE_2G;
    Wire.read(); Wire.read(); // skip temp bytes
    gx = (int16_t)((Wire.read() << 8) | Wire.read());
    gy = (int16_t)((Wire.read() << 8) | Wire.read());
    gz = (int16_t)((Wire.read() << 8) | Wire.read());
}
