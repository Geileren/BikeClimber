#include "GyroFilter.h"

GyroFilter::GyroFilter(IGyroConverter &gyro)
    : gyro_(gyro)
{
}

GyroFilter::Angles GyroFilter::filteredAngle()
{
    auto measurement = gyro_.readAll();

    auto roll = kalman1D(kalmanRollAngle, kalmanUncertaintyRollAngle, measurement.rollRate, measurement.rollAngle);

    kalmanRollAngle = roll.anglePrediction;
    kalmanUncertaintyRollAngle = roll.uncertainty;

    auto pitch = kalman1D(kalmanPitchAngle, kalmanUncertaintyPitchAngle, measurement.pitchRate, measurement.pitchAngle);

    kalmanPitchAngle = pitch.anglePrediction;
    kalmanUncertaintyPitchAngle = pitch.uncertainty;

    GyroFilter::Angles a;
    a.pitch = kalmanPitchAngle;
    a.roll = kalmanRollAngle;

    return a;
}

GyroFilter::KalmanOutput GyroFilter::kalman1D(double kalmanState, double kalmanUncertainty, double kalmanInput, double KalmanMeasurement)
{
    kalmanState = kalmanState + 0.004 * kalmanInput;
    kalmanUncertainty = kalmanUncertainty + 0.004 * 0.004 * 4 * 4;
    double kalmanGain = kalmanUncertainty * 1 / (1 * kalmanUncertainty + 3 * 3);
    kalmanState = kalmanState + kalmanGain * (KalmanMeasurement - kalmanState);
    kalmanUncertainty = (1 - kalmanGain) * kalmanUncertainty;

    return {kalmanState, kalmanUncertainty};
}
