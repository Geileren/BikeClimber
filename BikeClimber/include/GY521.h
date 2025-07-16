#pragma once

#include <Wire.h>

class GY521
{
public:
    enum AccelRange
    {
        low2g = 0,
        midLow4g = 1,
        midHigh8g = 2,
        high16g = 3
    };

    enum GyroRange
    {
        low250dps = 0,
        midLow500dps = 1,
        midHigh1000dps = 2,
        high2000dps = 3
    };

    struct GY521Config
    {
        uint8_t i2cAddress;
        uint8_t accelRange;
        uint8_t gyroRange;
    };

    struct GY521RawData
    {
        int16_t accX, accY, accZ;
        int16_t temp;
        int16_t gyroX, gyroY, gyroZ;
    };

    GY521();

    GY521(GY521Config config);
    void setupSensor();

    friend class GY521DataConverter;

    GY521RawData readAllRaw();
    int16_t readTempRaw();

private:
    uint8_t address;
    uint8_t accelRange;
    uint8_t gyroRange;

    void writeToRegistry(uint8_t registry, uint8_t value);
    uint8_t readFromRegistry(uint8_t registry);
};
