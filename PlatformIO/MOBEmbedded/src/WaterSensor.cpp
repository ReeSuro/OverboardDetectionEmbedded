#include "WaterSensor.hpp"

WaterSensor::WaterSensor(const uint8_t sensePin) : pin(sensePin)
{
}

bool WaterSensor::isWet()
{
    if (digitalRead(pin) == 1)
        return false;
    else
        return true;
}
