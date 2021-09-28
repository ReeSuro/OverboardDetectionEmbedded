//Main.cpp
//
// Author - Ree Surowiez
// Date - 09/07/21
#include <Arduino.h>
#define DEBUG
#include "main.hpp"
#include "Debug.hpp"
#include "MPU6050.hpp"
#include "Settings.hpp"
#include "NetworkManager.hpp"
#include "LEDManager.hpp"
#include "BatteryManager.hpp"
#include "WaterSensor.hpp"
#include "MOBDevice.hpp"

#ifndef UNIT_TESTS

MPU6050 accelerometer(threshold, counter);
WaterSensor ws(WATER_SENSE_PIN);
BatteryManager battery(VOLTAGE_SENSOR_PIN, EMPTY_ADC_VALUE, LOW_ADC_VALUE, MID_ADC_VALUE);
LEDManager leds(RED_LED, GREEN_LED, STROBE_LED);
NetworkSettings networkSettings(ssid, password, host, port, networkTimeout, socketTimeout, pingTime, pingMessage);
NetworkManager network(networkSettings);

MOBDevice device(&accelerometer, &battery, &leds, &ws, &network, LATCH_PIN, BUTTON_INT_PIN, WATER_SENSE_PERIOD, BUTTON_PUSH_PERIOD, THRESHOLD_FALL_TIME);

IRAM_ATTR void onFalling()
{
    device.falling();
}
IRAM_ATTR void onButton()
{
    device.buttonPress();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial working");
    attachInterrupt(digitalPinToInterrupt(ACC_INT_PIN), onFalling, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_INT_PIN), onButton, FALLING);
    device.start();
}

void loop()
{

}



#else

void setup()
{
    Serial.begin(9600);
    //Test LEDManager
    LEDManager leds(RED_LED, GREEN_LED, STROBE_LED);
    //Test the constructor
    if (leds.getRedPin() == RED_LED)
        Serial.println("RED LED PIN CORRECT");
    else
        Serial.println("RED LED PIN INCORRECT");

    if (leds.getGreenPin() == GREEN_LED)
        Serial.println("GREEN LED PIN CORRECT");
    else
        Serial.println("GREEN LED PIN INCORRECT");

    if (leds.getStrobePin() == STROBE_LED)
        Serial.println("STROBE LED PIN CORRECT");
    else
        Serial.println("STROBE LED PIN INCORRECT");

    if (digitalRead(RED_LED) == 0)
        Serial.println("RED LED PIN SET CORRECT");
    else
        Serial.println("RED LED PIN SET INCORRECT");

    if (digitalRead(GREEN_LED) == 0)
        Serial.println("GREEN LED PIN SET CORRECT");
    else
        Serial.println("GREEN LED PIN SET INCORRECT");

    if (digitalRead(STROBE_LED) == 0)
        Serial.println("STROBE LED PIN SET CORRECT");
    else
        Serial.println("STROBE LED PIN SET INCORRECT");

    //Test BatteryManager class
    BatteryManager bat(VOLTAGE_SENSOR_PIN, EMPTY_ADC_VALUE, LOW_ADC_VALUE, MID_ADC_VALUE);

    if (bat.getSensorPin() == A0)
        Serial.println("ADC PIN CORRECT");
    else
        Serial.println("ADC PIN INCORRECT");

    if (bat.getEmptyADCVal() == EMPTY_ADC_VALUE)
        Serial.println("EMPTY ADC VALUE CORRECT");
    else
        Serial.println("EMPTY ADC VALUE INCORRECT");

    if (bat.getLowADCVal() == LOW_ADC_VALUE)
        Serial.println("LOW ADC VALUE CORRECT");
    else
        Serial.println("LOW ADC VALUE INCORRECT");

    if (bat.getMidADCVal() == MID_ADC_VALUE)
        Serial.println("MID ADC VALUE CORRECT");
    else
        Serial.println("MID ADC VALUE INCORRECT");
}

void loop()
{
    delay(10000);
}

#endif
