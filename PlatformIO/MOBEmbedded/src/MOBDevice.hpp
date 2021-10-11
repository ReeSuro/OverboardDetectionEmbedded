//MOBDevice.hpp
//
//Author: Ree Surowiez
//Date: 08/05/2021
#pragma once
#include "MPU6050.hpp"
#include "WaterSensor.hpp"
#include "LEDManager.hpp"
#include "BatteryManager.hpp"
#include "NetworkManager.hpp"

//Enumeration of the different states
enum DeviceStates : int8_t
{
    POWEROFF,
    INIT,
    CONNECTING,
    OPERATING,
    EMERGENCY,
    SAFE_DISCONNECT,
    UNSAFE_DISCONNECT,
    FALLING_ANALYSIS
};

class MOBDevice
{
public:
    MOBDevice(MPU6050 *acc, BatteryManager *bat, LEDManager *led, WaterSensor *ws, NetworkManager *nm, const uint8_t latch, const uint8_t button, const uint16_t waterSensePeriod1, const uint16_t buttonPressPeriod1, const uint16_t fallThresholdTime1);
    void start();
    void debugStart();
    bool debugInitDevice();
    void falling();
    void buttonPress();

private:
    MPU6050 *acc;
    BatteryManager *bat;
    LEDManager *led;
    WaterSensor *ws;
    NetworkManager *nm;
    DeviceStates deviceState;
    const uint8_t latchPin;
    const uint8_t buttonPin;
    const uint16_t waterSensePeriod;
    const uint16_t buttonPressPeriod;
    const uint16_t fallThresholdTime;
};
