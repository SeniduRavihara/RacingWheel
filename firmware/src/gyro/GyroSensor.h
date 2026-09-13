#pragma once
#include <Arduino.h>
#include <Wire.h>

// ============================================================
//  GyroSensor.h
//  MPU-6050 interface: reads angular rate, integrates to angle
//  Uses complementary filter to reduce drift
// ============================================================

class GyroSensor {
public:
    GyroSensor();

    bool    begin();                    // Init I2C + MPU-6050, returns true if found
    void    calibrate(int samples = 500); // Capture gyro bias at startup (keep wheel still!)
    void    update();                   // Call this every loop — updates steering angle

    float   getSteeringAngle() const;   // Returns current angle in degrees (–540 to +540)
    float   getGyroZ() const;           // Raw Z-axis rate (rad/s)
    void    resetAngle();               // Force angle back to 0

    bool    isCalibrated() const { return _calibrated; }

private:
    float   _steeringAngle;             // Integrated steering angle (degrees)
    float   _gyroZBias;                 // Bias offset captured during calibration
    float   _gyroZ;                     // Current raw Z angular rate (rad/s)
    float   _accAngle;                  // Angle from accelerometer (degrees)
    unsigned long _lastTime;            // Timestamp of last update() call
    bool    _calibrated;

    void    readRaw(float &ax, float &ay, float &az,
                    float &gx, float &gy, float &gz);
};
