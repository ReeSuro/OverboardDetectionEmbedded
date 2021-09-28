//WaterSensor.cpp
//
//Author: Ree Surowiez
//Date:17/07/2021
#pragma once
#include "Arduino.h"

class WaterSensor
{
public:
    WaterSensor(const uint8_t sensePin);
    bool isWet();

private:
    const uint8_t pin;
};