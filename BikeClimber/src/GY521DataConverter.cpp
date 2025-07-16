#include "GY521DataConverter.h"

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

    auto raw = gyro_.readAllRaw();
    GyroData processedData;

    processedData.temp = convertTemp(raw.temp);

    processedData.accX = convertAcc(raw.accX);
    processedData.accY = convertAcc(raw.accY);
    processedData.accZ = convertAcc(raw.accZ);

    processedData.gyroX = convertGyro(raw.gyroX);
    processedData.gyroY = convertGyro(raw.gyroY);
    processedData.gyroZ = convertGyro(raw.gyroZ);

    return processedData;
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
