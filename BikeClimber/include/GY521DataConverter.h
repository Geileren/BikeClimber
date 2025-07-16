#pragma once

#include <Arduino.h>
#include "Interfaces/IGyroConverter.h"
#include "GY521.h"

class GY521DataConverter : public IGyroConverter
{
public:
    GY521DataConverter(GY521 gyro);

    double readTemp();
    GyroData readAll();

private:
    struct convGY521Data
    {
        double accX, accY, accZ;
        double temp;
        double gyroX, gyroY, gyroZ;
    };

    GY521 gyro_;

    convGY521Data convertData(GY521::GY521RawData *rawData);

    double convertTemp(int16_t rawTemp);
    double convertAcc(int16_t rawAcc);
    double convertGyro(int16_t rawGyro);

    double calcRollAngle(convGY521Data *procdData);
    double calcPitchAngle(convGY521Data *procdData);

    /*     double calcRollRate(convGY521Data *procdData);
        double calcPitchRate(convGY521Data *procdData);
        double calcYawRate(convGY521Data *procdData); */
};