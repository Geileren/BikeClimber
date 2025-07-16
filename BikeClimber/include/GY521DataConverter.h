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
    GY521 gyro_;
    double convertTemp(int16_t rawTemp);
    double convertAcc(int16_t rawAcc);
    double convertGyro(int16_t rawGyro);
};