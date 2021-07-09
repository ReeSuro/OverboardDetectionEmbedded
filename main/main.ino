// Main.ino
//
// Author: Ree Surowiez
// Date: 26/6/2021

#include <Wire.h>
#include <ESP8266WiFi.h>

//Pin defines
#define POWERLATCHPIN D5
#define REDLED
#define YELLOWLED
#define GREENLED
#define POWERBUTTON

//State defines
#define powerOffState 0
#define initState 1
#define operatingState 2
#define overboardState 3
#define disconnectingState 4
#define overboardDisconnectingState 5

//Debug Serial log macros
#define DEBUG
#ifdef DEBUG
#define DEBUGLOG(str) (Serial.print(str))
#define DEBUGLOGLN(str) (Serial.println(str))
#else
#define DEBUGLOG(str)
#define DEBUGLOGLN(str)
#endif

//Accelerometer MPU-6051 address'
const int16_t ACC_SLAVE_ADDR = 0x68;
const int16_t ACC_CE =

  //Function prototypes



void setup() {


  //Begin serial monitor if debug is defined
#ifdef DEBUG
  Serial.begin(9600);
#endif

}

void loop() {
  // put your main code here, to run repeatedly:

}




class Accelerometer
{
  private:
    int16_t ACC_SLAVE_ADDR;
    const int16_t ACC_CE_ADDR = 0x6B;
    const int16_t ACC_DATA_REG = 0x3B;
  public:

    Accelerometer(bool isAD0High)
    {
      //if ADO is high then the address is 0x69, if low then 0x68
      if (isAD0High)
        ACC_SLAVE_ADDR = 0x69;
      else
        ACC_SLAVE_ADDR = 0x68;
    }

    bool Init() {
      //Start I2C object
      Wire.begin();
      //Check the accelerometer address exists
      for (uint8_t i = 0; i < 5; i++) {
        //Check if the device exist by attempting communication
        Wire.beginTransmission(ACC_SLAVE_ADDR);
        byte error = Wire.endTransmission(false);
        if (error == 0) {
          //Start the accelerometer
          EnableCommunication();
          return true;
        }
      }
      return false;
    }
    void EnableCommunication() {
      Wire.beginTransmission(ACC_SLAVE_ADDR);
      Wire.write(ACC_CE_ADDR); //Power register
      Wire.write(0); //Pull to ground to turn on
      Wire.endTransmission(false);
    }
    void DisableCommunication() {
      Wire.beginTransmission(ACC_SLAVE_ADDR);
      Wire.write(ACC_CE_ADDR); //Power register
      Wire.write(1); //Pull high to turn off
      Wire.endTransmission(false);
    }

    void readAccelation() {
      
      
    }



}
