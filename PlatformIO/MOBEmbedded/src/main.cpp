//Main.cpp
//
// Author - Ree Surowiez
// Date - 09/07/21
#include <Arduino.h>

#include "Debug.hpp"
#include "MPU6050.hpp"
#include "Settings.hpp"
#include "NetworkManager.hpp"

DeviceStates deviceState = DeviceStates::POWEROFF;
MPU6050 accelerometer;
NetworkManager network;
WaterSensor ws;

void debugSetup(MPU6050 &accelerometer) 
{
    bool debugSuccessful = true;

    Serial.println("--------------------- Beginning Debug Checks ----------------------");
    Serial.println("Setting up MPU6050 - ");

    //Initialise and check the accelerometer
    Serial.print("Establishing communication: "); 
    bool acc_error = accelerometer.exists();
    Serial.println((acc_error ? "SUCCESS" : "FAILED"));
    //if communication is possible then continue setup
    if(acc_error)
    {
        accelerometer.begin();//Set up interrupts
        uint8_t[] acc_regs = accelerometer.getSettings();
        for(uint_t i = 0, i < 8, i++) 
        {
            Serial.println("POWER REGISTER 1  :" + acc_regs[0]);
            Serial.println("SIGNAL PATH RESET :" + acc_regs[1]);
            Serial.println("INT PIN REGISTER  :" + acc_regs[2]);
            Serial.println("HPF CONFIG REG    :" + acc_regs[3]);
            Serial.println("THRESHOLD REGISTER:" + acc_regs[4]);
            Serial.println("COUNTER REGISTER  :" + acc_regs[5]);
            Serial.println("DECREMENT REGISTER:" + acc_regs[6]);
            Serial.println("INT ENABLE REG    :" + acc_regs[7]);

        }


    }
    else
    {
        debugSuccessful = false;
    }
}



void setup()
{

  //1. Latch Power on
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
  DEBUG_LOGLN("Power Latch successful");

//2. Start Debug hardware checks
#ifdef DEBUG
  Serial.begin(9600);
  debugChecks();
#endif

  //initialize accelerometer and set up interrupt
  accelerometer.begin();
  attachInterruptToDigitalPin(digitalPinToInterrupt(), onFalling, FALLING);

  //3. Set the device state to connect to the socket
  deviceState = DeviceStates::CONNECTING;
}

void loop()
{

  switch (deviceState)
  {
  case POWEROFF:
//1. Clean any resources and memory before shutdown
#ifdef DEBUG
    Serial.close();
#endif
    //2. Disconnect the latch by pulling pin D5 low
    digitalWrite(LATCH_PIN, LOW);
    break;
  case CONNECTING:

    //1.Attempt connection to the network
    bool networkConnectStatus = network.networkConnect(networkTimeout, ssid, password);
    if (networkConnectStatus == 0)
    {
      //if network connection failed then go to the safe disconnect state
      deviceState = DeviceStates::SAFE_DISCONNECT;
      DEBUG_LOGLN("Network connection failed")
      break;
    }
    else
    {
      //if the network connection is successful then attempt connection to a socket
      bool socketConnectStatus = network.socketConnect();
      if (socketConnectStatus == 1)
      {
        //if the connection to the socket is successful then enter operating mode
        deviceState = DeviceStates::OPERATING;
        break;
      }
      else
      {
        deviceState = DeviceStates::SAFE_DISCONNECT;
        break;
      }
    }
    break;
  case OPERATING:

    //1. ping the socket
    network.sendPing();
    //2. Sleep while still operating
    delay(5000);

    //3. Read button
    //TODO: implement circuit for reading the push button

    //4. Read battery level
    readBattery();

    break;

  case EMERGENCY:

    break;
  case SAFE_DISCONNECT:
    break;
  case UNSAFE_DISCONNECT:
    break;
  }
}

void readBattery()
{


}



void onFalling()
{
  //Triggered when the device is falling
  //1. Wait until the device is not falling, TODO: ADD TIMER to measure fall time.
  while (accelerometer.isFalling());

  //2.  Read water sensor for a set period of time
  for (uint8_t i = 0; i < 10; i++)
  {
    if (isWater())
    {
      //if water is detected then enter emergency mode
      deviceState = DeviceStates::EMERGENCY;
    }
    else
    {
      delay(WATER_SENSE_PERIOD / 10);
    }
  }
}

enum DeviceStates : int8_t
{
  POWEROFF,
  CONNECTING,
  OPERATING,
  FREEFALL,
  EMERGENCY,
  SAFE_DISCONNECT,
  UNSAFE_DISCONNECT
};
