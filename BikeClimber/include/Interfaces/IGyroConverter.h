#include <Arduino.h>

class IGyroConverter
{
public:
    struct GyroData
    {
        double accX, accY, accZ;
        double temp;
        double gyroX, gyroY, gyroZ;
    };

    virtual GyroData readAll() = 0;
    virtual double readTemp() = 0;
    // virtual double[] readGyro() = 0;
    // virtual double[] readAccel() = 0;
};