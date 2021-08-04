//LEDManager.hpp
//
//Author: Ree Surowiez
//Date: 03/08/21
#include "LEDManager.hpp"

LEDManager::LEDManager(const unsigned char &red, const unsigned char &green, const unsigned char &strobe)
{
    //Store the pin values
    redPin = &red;
    greenPin = &green;
    strobePin = &strobe;

    //Set the led pins as output
    pinMode(*redPin, OUTPUT);
    pinMode(*greenPin, OUTPUT);
    pinMode(*strobePin, OUTPUT);

    //Set all leds to an off state
    digitalWrite(*redPin, LOW);
    digitalWrite(*greenPin, LOW);
    digitalWrite(*strobePin, LOW);
}

void LEDManager::strobe(const uint16_t &strobeTime)
{
    // Strobe the strobe led for the given time
    digitalWrite(*strobePin, HIGH);
    delay(strobeTime);
    digitalWrite(*strobePin, LOW);
}

void LEDManager::toggleRed()
{
    //toggle the red led
    if(digitalRead(*redPin))
        digitalWrite(*redPin, LOW);
    else    
        digitalWrite(*redPin, HIGH);
}

void LEDManager::toggleGreen()
{
    //toggle the red led
    if(digitalRead(*greenPin))
        digitalWrite(*greenPin, LOW);
    else    
        digitalWrite(*greenPin, HIGH);
}

void LEDManager::redOn()
{
    digitalWrite(*redPin, HIGH);
}

void LEDManager::greenOn()
{
    digitalWrite(*redPin, HIGH);
}

void LEDManager::redOff()
{
    digitalWrite(*redPin, LOW);
}

void LEDManager::greenOff()
{
    digitalWrite(*redPin, LOW);
}

const unsigned char LEDManager::getRedPin()
{
    return *redPin;
}
        
const unsigned char LEDManager::getGreenPin()
{
    return *greenPin;
}

const unsigned char LEDManager::getStrobePin()
{
    return *strobePin;
}
