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

DeviceStates deviceState = DeviceStates::POWEROFF;
MPU6050 accelerometer;
NetworkManager network;
//WaterSensor ws;
bool initDevice(MPU6050 accelerometer);

void debugNetworkConnect()
{
  //1.Attempt connection to the network
  bool networkConnectStatus = network.networkConnect(networkTimeout, ssid, password);
  if (networkConnectStatus == 0)
  {
    //if network connection failed then go to the safe disconnect state
    deviceState = DeviceStates::SAFE_DISCONNECT;
    DEBUG_LOGLN("Network connection failed")
  }
  else
  {
    //if the network connection is successful then attempt connection to a socket
    bool socketConnectStatus = network.socketConnect(host, port);
  }
}
  void setup()
  {
    //Latch Power on
    pinMode(LATCH_PIN, OUTPUT);
    digitalWrite(LATCH_PIN, HIGH);
    DEBUG_LOGLN("Power Latch successful");
    deviceState = DeviceStates::INIT;
//2. Start serial
#ifdef DEBUG
    Serial.begin(9600);
#endif
  }

  void loop()
  {

    switch (deviceState)
    {
    case POWEROFF:
    {

//1. Clean any resources and memory before shutdown
#ifdef DEBUG
      Serial.flush();
      Serial.end();
#endif
      //2. blink lights
      SetLEDS(true, false, false);
      delay(1000);
      SetLEDS(false, false, false);
      delay(1000);
      SetLEDS(true, false, false);
      delay(1000);
      SetLEDS(false, false, false);
      //3. Disconnect the latch by pulling pin D5 low
      digitalWrite(LATCH_PIN, LOW);
      break;
    }
    case INIT:
    {
      //Initialise sensors
      bool result = initDevice(accelerometer);
      if (result)
        deviceState = DeviceStates::POWEROFF;
      else
        deviceState = DeviceStates::POWEROFF;
    }
    case CONNECTING:
    {
    break;
    }
  case OPERATING:
  {
    //1. ping the socket
    network.sendPing("A");
    //2. Sleep while still operating
    delay(pingTime);

    //3. Read button
    //TODO: implement circuit for reading the push button

    //4. Read battery level
    //readBattery();

    break;
  }
  case FREEFALL:
  {
    break;
  }
  case EMERGENCY:
  {
    break;
  }
  case SAFE_DISCONNECT:
  {
    break;
  }
  case UNSAFE_DISCONNECT:
  {
    break;
  }
  }
}

void readBattery()
{
}

void SetLEDS(bool red, bool yellow, bool green)
{
  digitalWrite(RED_LED, red);
  digitalWrite(YELLOW_LED, yellow);
  digitalWrite(GREEN_LED, green);
}

IRAM_ATTR void onFalling()
{
  //Triggered when the device is falling
  //1. Wait until the device is not falling, TODO: ADD TIMER to measure fall time.
  while (accelerometer.isFalling())
    ;

  //2.  Read water sensor for a set period of time
  for (uint8_t i = 0; i < 10; i++)
  {
    if (!digitalRead(WATER_SENSE_PIN))
    {
      //if water is detected then enter emergency mode
      deviceState = DeviceStates::EMERGENCY;
      break;
    }
    else
    {
      delay(WATER_SENSE_PERIOD / 10);
    }
  }
}
IRAM_ATTR void onButton(){}
bool initDevice(MPU6050 accelerometer)
{

#ifdef DEBUG
  bool debugSuccessful = true;
  Serial.println("--------------------- Beginning Debug Checks ----------------------");
  Serial.println("Setting up MPU6050 - ");

  //Initialise and check the accelerometer
  Serial.println("Establishing communication: ");
  bool acc_error = accelerometer.exists();
  Serial.println((acc_error ? "SUCCESS" : "FAILED"));
  //if communication is possible then continue setup
  if (acc_error)
  {
    Serial.println("Setting up accelerometer - ");
    accelerometer.begin(threshold, counter); //Set up interrupts
    uint8_t *acc_regs = accelerometer.getSettings();
    Serial.print("POWER REGISTER 1  :");
    Serial.println(acc_regs[0], BIN);
    Serial.print("SIGNAL PATH RESET :");
    Serial.println(acc_regs[1], BIN);
    Serial.print("INT PIN REGISTER  :");
    Serial.println(acc_regs[2], BIN);
    Serial.print("HPF CONFIG REG    :");
    Serial.println(acc_regs[3], BIN);
    Serial.print("THRESHOLD REGISTER:");
    Serial.println(acc_regs[4], BIN);
    Serial.print("COUNTER REGISTER  :");
    Serial.println(acc_regs[5], BIN);
    Serial.print("DECREMENT REGISTER:");
    Serial.println(acc_regs[6], BIN);
    Serial.print("INT ENABLE REG    :");
    Serial.println(acc_regs[7], BIN);
    Serial.println("");
    free(acc_regs);
    acc_regs = NULL;
  }
  else
  {
    debugSuccessful = false;
  }
  delay(1000);
#else
  accelerometer.begin(threshold, counter); //Set up free fall interrupt
#endif

#ifdef DEBUG
  Serial.println("Testing LEDS - ");
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  SetLEDS(false, false, false);
  Serial.println("RED LED ON: NOW ");
  digitalWrite(RED_LED, HIGH);
  delay(2000);
  digitalWrite(RED_LED, LOW);
  Serial.println("YELLOW LED ON: NOW ");
  digitalWrite(YELLOW_LED, HIGH);
  delay(2000);
  digitalWrite(YELLOW_LED, LOW);
  Serial.println("GREEN LED ON: NOW ");
  digitalWrite(GREEN_LED, HIGH);
  delay(2000);
  digitalWrite(GREEN_LED, LOW);
  Serial.println("All LEDs ON: NOW ");
  SetLEDS(true, true, true);
  delay(2000);
  SetLEDS(true, false, false);
#else
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  SetLEDS(true, false, false);
#endif
//Test water sensor
#ifdef DEBUG
  Serial.println("Testing Water Sensor - ");

  if (digitalRead(WATER_SENSE_PIN) == 0)
  {
    Serial.println("Please seperate water sensor terminals:");
    while (digitalRead(WATER_SENSE_PIN) == 0)
      ;
    delay(500);
  }
  Serial.println("Contact water sense terminals:");
  bool contacted = false;
  for (uint8_t i = 0; i < 20; i++)
  {
    if (digitalRead(WATER_SENSE_PIN) == 0)
    {
      contacted = true;
      break;
    }
    delay(500);
  }
  Serial.print("WATER SENSOR SETUP: ");
  Serial.println(contacted ? "SUCCESS" : "FAILED");
  if (!contacted)
  {
    debugSuccessful = false;
  }
#endif
  //TODO: ADD CHECKS FOR BATTERY VOLTAGE SENSOR

  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(ACC_INT_PIN), onFalling, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT_PIN), onButton, FALLING);

#ifdef DEBUG
  if (debugSuccessful){
    Serial.println("Debug checks successful");
  return 1;
  }
  else {
    Serial.println("Debug checks failed");
  return 0;
  }
#else
  pinMode(WATER_SENSE_PIN, INPUT);
  return 1;
#endif
}
