
#include <Arduino_LSM6DS3.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
}

void loop() {
  float gx, gy, gz;
  float ax, ay, az;

  if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
    IMU.readAcceleration(ax, ay, az);

    String data = String(ax, 2) + "," + String(ay, 2) + "," + String(az, 2) + ","
                + String(gx, 2) + "," + String(gy, 2) + "," + String(gz, 2);

    byte checksum = 0;
    for (int i = 0; i < data.length(); i++) {
      checksum ^= data.charAt(i);
    }

    Serial.print(data);
    Serial.print('*');
    if (checksum < 0x10) Serial.print('0');  
    Serial.println(String(checksum, HEX));

    delay(16);
  }
}
