// ============================================================
//  🧪 TEST 1: I2C Scanner
//  Verifies physical connection between ESP32-S3 and MPU-6050
//
//  Wiring:
//    - MPU-6050 VCC -> ESP32-S3 3.3V
//    - MPU-6050 GND -> ESP32-S3 GND
//    - MPU-6050 SDA -> ESP32-S3 GPIO 8
//    - MPU-6050 SCL -> ESP32-S3 GPIO 9
//
//  Expected Result:
//    - Should report "I2C device found at address 0x68" (or 0x69)
// ============================================================

#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor to connect

  Serial.println("\n==================================");
  Serial.println("   ESP32-S3 I2C Bus Scanner");
  Serial.println("==================================");

  Wire.begin(SDA_PIN, SCL_PIN);
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning I2C bus (SDA = GPIO 8, SCL = GPIO 9)...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("  [SUCCESS] I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);

      if (address == 0x68) {
        Serial.println("  <-- MPU-6050 (Default AD0 -> GND)");
      } else if (address == 0x69) {
        Serial.println("  <-- MPU-6050 (AD0 -> 3.3V)");
      } else {
        Serial.println();
      }
      nDevices++;
    } else if (error == 4) {
      Serial.print("  [ERROR] Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("  ❌ No I2C devices found!");
    Serial.println("     Check: VCC (3.3V), GND, SDA (GPIO 8), SCL (GPIO 9)");
  } else {
    Serial.println("Done. Scan complete.\n");
  }

  delay(3000); // Rescan every 3 seconds
}
