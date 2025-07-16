#include "GY521DataConverter.h"

// Calibration offsets found by testing the sensor (these are individual per sensor)
#define GYRO_OFFSET_X -0.04
#define GYRO_OFFSET_Y 0.01
#define GYRO_OFFSET_Z 0.00

#define ACC_OFFSET_X 0.13
#define ACC_OFFSET_Y 0.00
#define ACC_OFFSET_Z 0.01

GY521DataConverter::GY521DataConverter(GY521 gyro)
{
    gyro_ = gyro;
}

double GY521DataConverter::readTemp()
{
    return convertTemp(gyro_.readTempRaw());
}

GY521DataConverter::GyroData GY521DataConverter::readAll()
{

    auto rawData = gyro_.readAllRaw();
    auto convData = convertData(&rawData);

    IGyroConverter::GyroData procdData;

    procdData.rollAngle = calcRollAngle(&convData);
    procdData.pitchAngle = calcPitchAngle(&convData);

    procdData.rollRate = convData.accX;
    procdData.pitchRate = convData.accY;
    procdData.yawRate = convData.accZ;

    procdData.temp = convData.temp;

    return procdData;
}

GY521DataConverter::convGY521Data GY521DataConverter::convertData(GY521::GY521RawData *rawData)
{
    convGY521Data convData;

    convData.temp = convertTemp(rawData->temp);

    convData.accX = convertAcc(rawData->accX) - ACC_OFFSET_X;
    convData.accY = convertAcc(rawData->accY) - ACC_OFFSET_Y;
    convData.accZ = convertAcc(rawData->accZ) - ACC_OFFSET_Z;

    convData.gyroX = convertGyro(rawData->gyroX) - GYRO_OFFSET_X;
    convData.gyroY = convertGyro(rawData->gyroY) - GYRO_OFFSET_Y;
    convData.gyroZ = convertGyro(rawData->gyroZ) - GYRO_OFFSET_Z;

    return convData;
}

double GY521DataConverter::convertTemp(int16_t rawTemp)
{
    return rawTemp / 340.0 + 36.53;
}
double GY521DataConverter::convertAcc(int16_t rawAcc)
{
    double LSBSensitivity = pow(2, 14 - gyro_.accelRange); // sens gets smaller with bigger range
    return rawAcc / LSBSensitivity;
}
double GY521DataConverter::convertGyro(int16_t rawGyro)
{
    double LSBSensitivity;
    switch (gyro_.gyroRange)
    {
    case 0:
        LSBSensitivity = 131.0;
        break;
    case 1:
        LSBSensitivity = 65.5;
        break;
    case 2:
        LSBSensitivity = 32.8;
        break;
    case 3:
        LSBSensitivity = 16.4;
        break;
    }
    return rawGyro / LSBSensitivity;
}

double GY521DataConverter::calcRollAngle(convGY521Data *procdData)
{
    double oppSide = procdData->accY;
    double adjSide = sqrt(pow(procdData->accX, 2) + pow(procdData->accZ, 2)); // pythogoras

    double rollAngleRad = atan(oppSide / adjSide);
    double rollAngle = rollAngleRad * 180 / PI; // convert to degrees
    return rollAngle;
}

double GY521DataConverter::calcPitchAngle(convGY521Data *procdData)
{
    double oppSide = -procdData->accX;
    double adjSide = sqrt(pow(procdData->accY, 2) + pow(procdData->accZ, 2)); // pythogoras

    double pitchAngleRad = atan(oppSide / adjSide);
    double pitchAngle = pitchAngleRad * 180 / PI; // convert to degrees
    return pitchAngle;
}

/* double GY521DataConverter::calcRollRate(convGY521Data *procdData)
{
    return 0.0;
}

double GY521DataConverter::calcPitchRate(convGY521Data *procdData)
{
    return 0.0;
}

double GY521DataConverter::calcYawRate(convGY521Data *procdData)
{
    return 0.0;
} */
