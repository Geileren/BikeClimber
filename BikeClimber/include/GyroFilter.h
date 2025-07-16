#pragma once

#include <Arduino.h>
#include "GY521DataConverter.h"

class GyroFilter
{
public:
    struct Angles
    {
        double roll;
        double pitch;
    };

    GyroFilter(IGyroConverter &gyro);

    GyroFilter::Angles filteredAngle();

private:
    struct KalmanOutput
    {
        double anglePrediction;
        double uncertainty;
    };
    IGyroConverter &gyro_;

    KalmanOutput kalman1D(double kalmanState, double kalmanUncertainty, double kalmanInput, double KalmanMeasurement);

    double kalmanRollAngle = 0,
           kalmanUncertaintyRollAngle = 2 * 2;
    double kalmanPitchAngle = 0,
           kalmanUncertaintyPitchAngle = 2 * 2;
};