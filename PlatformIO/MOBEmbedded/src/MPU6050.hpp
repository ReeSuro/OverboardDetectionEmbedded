#include <Wire.h>

#define ACC_ADDR            0x68 //Address of the accelerometer when AD0 is pulled to ground
#define POWER_REG_1         0x6B //Power controls device reset, sleep and clock source
#define SIGNAL_PATH_RESET   0x68 //Used to reset the signal paths for the accelerometer, gyro and thermometer
#define INT_PIN_CFG         0x37 //Configure the interrupt pins
#define HPF_CONFIG          0x1C //Configure the high pass filter cut off frequency
#define FF_THRESHOLD_REG    0x1D //Set the threshold reading for free fall
#define FF_COUNTER_REG      0x1E //Set the counter reading for free fall
#define FF_DETECT_CTRL      0x69 //Set the free fall detection behavior
#define INT_ENABLE_REG      0x38 //Enable interrupts

class MPU6050 
{
    public:
        void begin();
        void end();
        bool exists();
    private:
        uint8_t writeRegister(uint8_t addr, uint8_t reg, uint8_t data);
        uint8_t readRegister(uint8_t addr, uint8_t reg);
        uint8_t writeBit(uint8_t addr, uint8_t reg, uint8_t bitN, bool state);
};



