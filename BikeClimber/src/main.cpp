
#include <Arduino.h>
#include <Wire.h>
#include "GY521.h"
#include "GY521DataConverter.h"
#include <GyroFilter.h>

#define BAUD_RATE 9600
#define MPU_I2C_ADDRESS 0x68

GY521::GY521Config gyro1Config{
    MPU_I2C_ADDRESS,
    GY521::AccelRange::low2g,
    GY521::GyroRange::low250dps};

GY521 gyro1(gyro1Config);
GY521DataConverter gyroConverter1(gyro1);

GyroFilter gf(gyroConverter1);

void setup()
{
  Wire.begin();
  Serial.begin(BAUD_RATE);
  gyro1.setupSensor();
}

void loop()
{
  auto measurement = gf.filteredAngle();

  Serial.print("roll:");
  Serial.print(measurement.roll);
  Serial.print(",");

  Serial.print("pitch:");
  Serial.println(measurement.pitch);
  // Serial.print("  ");

  /*  Serial.print("gX: ");
   Serial.print(measurement.gyroX);
   Serial.print("  ");

   Serial.print("gY: ");
   Serial.print(measurement.gyroY);
   Serial.print("  ");

   Serial.print("gZ: ");
   Serial.print(measurement.gyroZ);
   Serial.println("  "); */

  delay(10);
}
