#include "Arduino.h"

class LEDManager
{

    public:
        LEDManager(const unsigned char &red, const unsigned char &green, const unsigned char &strobePin);
        void strobe(const unsigned short &strobeTime);
        void toggleRed();
        void toggleGreen();
        void redOn();
        void redOff();
        void greenOn();
        void greenOff();
        const unsigned char getRedPin();
        const unsigned char getGreenPin();
        const unsigned char getStrobePin();

    private:
        const unsigned char* redPin;
        const unsigned char* greenPin;
        const unsigned char* strobePin;     
};

