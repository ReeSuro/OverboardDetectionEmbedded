//  BatteryManager.hpp
//  Description: Used to interface with the battery voltage sensor on the MOBDevice through a 10 bit ADC
//  Author: Ree Surowiez
//  Date: 05/08/21

class BatteryManager
{
    public:
        BatteryManager(const uint8_t sensePin, const int empty, const int low, const int mid);
        bool isVoltageSafe(); //To determine if the voltage is above a required level for safe opeation
        String getVoltageAsString();//Returns the battery state as a string of 'H', 'M', or 'L', for high, medium, or low respectively
        const uint8_t getSensorPin();//Returns the sensor ADC pin as an 8-bit integer
        const int getEmptyADCVal();
        const int getLowADCVal();
        const int getMidADCVal();
        
    private:
        const uint8_t sensPin;//The ADC Pin on the ESP-12
        const int emptyADCVal;//ADC reading signalling an empty battery
        const int lowADCVal;//ADC reading signalling a low battery
        const int midADCVal;//ADC reading signalling a mid battery

        int readVoltage();//Returns a 10 bit reading from the ADC
        char parseVoltage(int& rawVoltage);//Returns a character representing the battery state
      
};