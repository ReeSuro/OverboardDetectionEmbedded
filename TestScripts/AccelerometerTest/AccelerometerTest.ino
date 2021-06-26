#include "Wire.h"

const int acc_addr = 0x68; //Address of the accelerometer when AD0 is pulled to ground
bool accConnected = false;
int16_t aX, aY, aZ, temp, gX, gY, gZ;

void setup() {
  //Start I2C object
  Wire.begin();
  Serial.begin(9600);

  //Check the accelerometer address
  for (uint8_t i = 0; i < 5; i++) {
    accConnected = AccExists(acc_addr);
    if (accConnected) {
      //Start the accelerometer
      enableAcc(acc_addr);
      break;
    }
    else
      Serial.println("Device not connected");
    delay(1000);
  }
}

void loop() {
  //if the device is not connected then end
  if (!accConnected) {
    return;
  }
  while (true) {

    //Read the values
    Wire.beginTransmission(acc_addr);
    //Write start register
    Wire.write(0x3B);
    Wire.endTransmission(false);//End write but keep connection open
    Wire.requestFrom(acc_addr, 14, true);//request 14 bytes
    //Each variable is 2 bytes therefore each byte must be shifted before reading the next one.
    aX = Wire.read() << 8 | Wire.read();
    aY = Wire.read() << 8 | Wire.read();
    aZ = Wire.read() << 8 | Wire.read();
    temp = Wire.read() << 8 | Wire.read();
    gX = Wire.read() << 8 | Wire.read();
    gY = Wire.read() << 8 | Wire.read();
    gZ = Wire.read() << 8 | Wire.read();
    Serial.print("AcX = "); Serial.print(aX);
    Serial.print(" | aY = "); Serial.print(aY);
    Serial.print(" | aZ = "); Serial.print(aZ);
    Serial.print(" | Temp = "); Serial.print(temp);
    Serial.print(" | gX = "); Serial.print(gX);
    Serial.print(" | gY = "); Serial.print(gY);
    Serial.print(" | gZ = "); Serial.println(gZ);

    delay(1000);
  }

}


bool AccExists(const int addr)
{
  //Check if the device exist by attempting communication
  Wire.beginTransmission(addr);
  byte error = Wire.endTransmission(true);
  if (error == 0)
    return true;
  else
    return false;
}

void enableAcc(const int addr)
{
  Wire.beginTransmission(addr);
  Wire.write(0x6B); //Power register
  Wire.write(0); //Pull to ground to turn on
  Wire.endTransmission(true);
}
