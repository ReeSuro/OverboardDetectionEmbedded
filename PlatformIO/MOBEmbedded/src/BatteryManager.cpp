//  BatteryManager.cpp
//  Description: Definitions of the BatteryManager class
//  Author: Ree Surowiez
//  Date: 05/08/2021

#include "BatteryManager.hpp"

BatteryManager::BatteryManager(const uint8_t sensPin, const int empty, const int low, const int mid) : sensePin(sensPin),
                                                                                                       emptyADCVal(empty),
                                                                                                       lowADCVal(low),
                                                                                                       midADCVal(mid)
{
    //Initialise the pin
    pinMode(sensePin, INPUT);
}

bool BatteryManager::isVoltageSafe()
{
    if (analogRead(sensePin) < emptyADCVal)
        return false;
    else
        return true;
}

String BatteryManager::getVoltageAsString()
{
    int val = readVoltage();
    return (String)parseVoltage(val);
}

const uint8_t BatteryManager::getSensorPin()
{
    return sensePin;
}

int BatteryManager::readVoltage()
{
    return analogRead(sensePin);
}

char BatteryManager::parseVoltage(int &rawADC)
{
    //convert the adc reading to a low, medium or high output give the ranges
    if (rawADC >= midADCVal)
        return 'H';
    else if (rawADC >= lowADCVal)
        return 'M';
    else if (rawADC < lowADCVal)
        return 'L';
    else
        return 'F';
}

const int BatteryManager::getLowADCVal()
{
    return lowADCVal;
}

const int BatteryManager::getMidADCVal()
{
    return midADCVal;
}

const int BatteryManager::getEmptyADCVal()
{
    return emptyADCVal;
}