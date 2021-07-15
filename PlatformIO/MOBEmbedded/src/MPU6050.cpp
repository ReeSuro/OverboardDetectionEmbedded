//MPU6050.cpp
//
//Author: Ree Surowiez
//Date: 10/07/2021

#include "MPU6050.hpp"
#include "Debug.hpp"

class MPU6050
{
public:
    MPU6050()
    {
        //Start I2C object
        Wire.begin();
#ifdef DEBUG
        Serial.begin(9600);
#endif
    }

    void begin(uint8_t &threshold, uint8_t &counter)
    {
        //configure free fall interrupt
        writeRegister(ACC_ADDR, POWER_REG_1, 0b00000000); //Wake device
        writeRegister(ACC_ADDR, SIGNAL_PATH_RESET, 0b00000111);//Reset all analogue and digital signal paths
        writeRegister(ACC_ADDR, INT_PIN_CFG, 0b10100000); //Interrupt active low, push-pull, high until read, clear on read.
        writeRegister(ACC_ADDR, HPF_CONFIG, 0b00000001); // 5hz high pass 
        writeRegister(ACC_ADDR, FF_THRESHOLD_REG, threshold);// Set threshold in mg
        writeRegister(ACC_ADDR, FF_COUNTER_REG, counter); // Set counter in ms
        writeRegister(ACC_ADDR, FF_DETECT_CTRL, 0b00010101); //FF dcrements 1mg per ms
        writeRegister(ACC_ADDR, INT_ENABLE_REG, 0b10000000); //Enable free fall interrupt
    }

    bool exists()
    {
        //attempt to communicate with the device
        Wire.beginTransmission(ACC_ADDR);
        uint8 error = Wire.endTransmission(true);
        if (error == 0)
            return true;
        else
            return false;
    }

    void end()
    {
        //Disable the free fall interrupt
        writeRegister(ACC_ADDR, INT_ENABLE_REG, 0b00000000);
    }

private:
    uint8_t writeRegister(uint8_t addr, uint8_t reg, uint8_t data)
    {

        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(data);
        uint8_t error = Wire.endTransmission(true);
        return error;
    }

    uint8_t readRegister(uint8_t addr, uint8_t reg)
    {

        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.endTransmission(false);
        Wire.beginTransmission(addr);
        Wire.requestFrom(addr, 1, true);
        uint8_t data = Wire.read();
        Wire.endTransmission(true);
        return data;
    }

    uint8_t writeBit(uint8_t addr, uint8_t reg, uint8_t bitN, bool state)
    {
        uint8_t value = readRegister(addr, reg);

        if (state)
            value |= (1 << bitN);
        else
            value &= ~(1 << bitN);

        return writeRegister(addr, reg, value);
    }
};