#include "GY521.h"
#include <Wire.h>
#include <Arduino.h>

// Register address definitions (from datasheet)
#define ACCEL_CONFIG 0x1C // Accelerometer range/sensitivity config
#define TEMP_OUT_H 0x41   // Temperature measurement upper byte
#define TEMP_OUT_L 0x42   // Temperature measurement lower byte
#define PWR_MGMT_1 0x6B   // Power Mangement 1

#define MEASUREMENT_START_REGISTER 0x3B // First measurement registry

GY521::GY521()
{
    address = 0x68;
    accelRange = 0;
    gyroRange = 0;
}

GY521::GY521(GY521::GY521Config config)
{
    address = config.i2cAddress;
    accelRange = config.accelRange;
    gyroRange = config.gyroRange;
}

void GY521::setupSensor()
{
    writeToRegistry(PWR_MGMT_1, 0x00); // wake up
    writeToRegistry(ACCEL_CONFIG, accelRange << 3);
}

int16_t GY521::readTempRaw()
{
    uint8_t tempH = readFromRegistry(TEMP_OUT_H);
    uint8_t tempL = readFromRegistry(TEMP_OUT_L);
    return (tempH << 8) | tempL;
}

void GY521::writeToRegistry(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t GY521::readFromRegistry(uint8_t reg)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available())
    {
        return Wire.read();
    }
    return 0;
}

GY521::GY521RawData GY521::readAllRaw()
{

    uint8_t buffer[14];

    Wire.beginTransmission(address);
    Wire.write(MEASUREMENT_START_REGISTER);
    Wire.endTransmission(false); // Repeated start

    Wire.requestFrom(address, (uint8_t)14);

    for (uint8_t i = 0; i < 14 && Wire.available(); i++)
    {
        buffer[i] = Wire.read();
    }

    GY521::GY521RawData data;

    data.accX = (int16_t)(buffer[0] << 8 | buffer[1]);
    data.accY = (int16_t)(buffer[2] << 8 | buffer[3]);
    data.accZ = (int16_t)(buffer[4] << 8 | buffer[5]);
    data.temp = (int16_t)(buffer[6] << 8 | buffer[7]);
    data.gyroX = (int16_t)(buffer[8] << 8 | buffer[9]);
    data.gyroY = (int16_t)(buffer[10] << 8 | buffer[11]);
    data.gyroZ = (int16_t)(buffer[12] << 8 | buffer[13]);

    return data;
}
