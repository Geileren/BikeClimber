#pragma once

#include <Arduino.h>

class IGyroConverter
{
public:
    struct GyroData
    {
        double rollAngle, pitchAngle;
        double temp;
        double rollRate, pitchRate, yawRate; // (x, y, z)
    };

    virtual GyroData readAll() = 0;
    virtual double readTemp() = 0;
    // virtual double[] readGyro() = 0;
    // virtual double[] readAccel() = 0;
};