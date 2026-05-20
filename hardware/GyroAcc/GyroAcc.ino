#include <Arduino_LSM6DS3.h>

byte checksum;

void printWithChecksum(float val, bool last) {
  char buf[12];
  sprintf(buf, "%.2f", val);
  for (int i = 0; buf[i] != '\0'; i++) {
    checksum ^= buf[i];
  }
  Serial.print(buf);
  if (!last) {
    checksum ^= ',';
    Serial.print(',');
  }
}

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

    checksum = 0;

    printWithChecksum(ax, false);
    printWithChecksum(ay, false);
    printWithChecksum(az, false);
    printWithChecksum(gx, false);
    printWithChecksum(gy, false);
    printWithChecksum(gz, true);

    Serial.print('*');
    if (checksum < 0x10) Serial.print('0');
    Serial.println(String(checksum, HEX));

    delay(10);
  }
}
