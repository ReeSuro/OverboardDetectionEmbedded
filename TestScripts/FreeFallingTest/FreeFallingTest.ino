
#include "Wire.h"

const int addr = 0x68;
void onFalling();
uint8_t writeBit(uint8_t reg, uint8_t bitN, bool state);
uint8_t writeRegister(uint8_t reg, uint8_t data);
uint8_t readRegister(uint8_t reg);

void setup() {
 Serial.begin(115200);
 Serial.println("Serial Online");
  //Start I2C communication 
  Wire.begin();
 
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);
   digitalWrite(D6, HIGH);
   delay(1000);
   digitalWrite(D6, LOW);
  
  //attempt communication 
  Wire.beginTransmission(addr);
  uint8_t err =  Wire.endTransmission();
  if(err)
    Serial.println("Cannot connect");
  else
    Serial.println("Connection successful");
  //enable the chip 
  writeRegister(0x6B, 0);
  Serial.print("Chip setting register is set to(0x6B): "); Serial.println(readRegister(0x6B)); 
  writeRegister(0x68, 0b111);
  Serial.print("Chip setting register is set to(0x6B): "); Serial.println(readRegister(0x68)); 
  //Setup the interrupt on the MPU
  //set a delay on the acc startup
 // uint8_t val = readRegister(0x69);
 // val &= 0b11001111;
//  val |= ((0b11) << 4);
  writeRegister(0x69, 0b10101);
  Serial.println("Accelerometer setting register is set to(0x69): "); Serial.println(readRegister(0x69)); 
  //Configure the onboard highpass filter to 5HZ
  //val = readRegister(0x1C);
 // val &= 0b11111000;//Set the first three bits to 0
 // val |= 0b00000001;//Set the dhpf to 5HZ mode
  writeRegister(0x1C, 0b1);
  Serial.println("High pass filter setting register is set to(0x1C): "); Serial.println(readRegister(0x1C)); 
  //enable the free fall interrupt
  writeRegister(0x38, 0b10000000);
  Serial.println("interrupt type setting register is set to(0x38): "); Serial.println(readRegister(0x38)); 
  //set the threshold value
  writeRegister(0x1D, 17);
  Serial.println("Threshhold value register is set to(0x1D): "); Serial.println(readRegister(0x1D)); 
  //Set the counter value
  writeRegister(0x1E, 0);
  Serial.println("Counter setting register is set to(0x1E): "); Serial.println(readRegister(0x1E)); 
  writeRegister(0x37, 0b1011000);
  Serial.println("Interrupt setting register is set to(0x1E): "); Serial.println(readRegister(0x37)); 

  //Attach the interrupt
  
  attachInterrupt(digitalPinToInterrupt(D8), onFalling, RISING);
}

uint8_t readRegister(uint8_t reg) {

  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.beginTransmission(addr);
  Wire.requestFrom(addr, 1 , true);
  uint8_t data = Wire.read();
  Wire.endTransmission(true);
  return data;
}

uint8_t writeRegister(uint8_t reg, uint8_t data) {

  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(data);
  uint8_t error = Wire.endTransmission(true);
  return error;
}

uint8_t writeBit(uint8_t reg, uint8_t bitN, bool state) 
{

  uint8_t value = readRegister(reg);

  if(state)
    value |= (1 << bitN);
  else
    value &= ~(1 << bitN);   
  
  return writeRegister(reg, value);
}

 ICACHE_RAM_ATTR void onFalling() {
  
  Serial.println("Interrupt") ;
}


void loop() {
  Serial.print("Interrupt flags(0x3A): "  ); Serial.println( readRegister(0x3A));
  digitalWrite(D6, HIGH);
  delay(1000);
  digitalWrite(D6, LOW);
  delay(1000);
}
