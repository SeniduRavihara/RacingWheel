# 🎯 Calibration Guide

## Auto-Calibration (On Every Boot)

The firmware auto-calibrates on every startup:

1. Power on the ESP32-S3
2. LED will turn **solid ON** for ~1 second
3. During this time — **keep the wheel completely still!**
4. Calibration captures the gyro's zero-rate bias (removes baseline drift)
5. LED stays solid = calibration done, ready to race!

## Manual Re-center (During Use)

If steering drifts during a session:
- Simply hold the wheel straight and **stay still for 2-3 seconds**
- The anti-drift algorithm will gently pull the angle back to 0°
- Or press a button combo (configure in `Config.h`) to force reset

## Tuning Parameters (in Config.h)

| Parameter | Default | Effect |
|-----------|---------|--------|
| `GYRO_DRIFT_THRESHOLD` | 0.05 rad/s | How "still" = stationary |
| `DRIFT_CORRECTION` | 0.995 | Re-center speed (lower = faster) |
| `COMPLEMENTARY_ALPHA` | 0.98 | Gyro vs accel blend |
| `STEERING_MAX_DEG` | 540° | Max wheel rotation |

## Placement of MPU-6050

Mount the MPU-6050 **flat on the wheel face**, with the chip's Y-axis pointing **up** (toward 12 o'clock).  
This ensures Z-axis rotation = steering yaw.
