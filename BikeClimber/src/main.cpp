
#include <Arduino.h>
#include <Wire.h>
#include "GY521.h"
#include "GY521DataConverter.h"

#define BAUD_RATE 9600
#define MPU_I2C_ADDRESS 0x68

GY521::GY521Config gyro1Config{
    MPU_I2C_ADDRESS,
    GY521::AccelRange::low2g,
    GY521::GyroRange::low250dps};

GY521 gyro1(gyro1Config);
GY521DataConverter gyroConverter1(gyro1);

void setup()
{
  Wire.begin();
  Serial.begin(BAUD_RATE);
  gyro1.setupSensor();
}

void loop()
{
  GY521DataConverter::GyroData measurement = gyroConverter1.readAll();

  Serial.print(measurement.accX);
  Serial.print(",");
  Serial.println(measurement.gyroX);

  /*   Serial.print("aY: ");
    Serial.print(measurement.accY);
    Serial.print("  ");

    Serial.print("aZ: ");
    Serial.print(measurement.accZ);
    Serial.print("  ");

    Serial.print("gX: ");
    Serial.print(measurement.gyroX);
    Serial.print("  ");

    Serial.print("gY: ");
    Serial.print(measurement.gyroY);
    Serial.print("  ");

    Serial.print("gZ: ");
    Serial.print(measurement.gyroZ);
    Serial.print("  ");
   */
  delay(20);
}
