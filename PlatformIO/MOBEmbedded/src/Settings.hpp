//Pin defines
#define LATCH_PIN D8
#define WATER_SENSE_PIN D7
#define ACC_INT_PIN D6
#define BUTTON_INT_PIN D5
#define RED_LED D4
#define GREEN_LED D3
#define I2C_SDA_PIN D2
#define I2C_SCL_PIN D1
#define STROBE_LED D0
#define VOLTAGE_SENSOR_PIN A0

//Network settings
const char* ssid = "NA";
const char* password = "116028E44B";
const char* host = "192.168.1.100";
const uint16_t port = 11000;
const uint16_t networkTimeout = 10; //Timeout for connection to the network in seconds 
const uint16_t socketTimeout = 10; // Timeour for conncetion to the socket in seconds 
const uint16_t pingTime = 5000;
#define pingMessage "A"
//accelerometer settings
const uint8_t threshold = 100;
const uint8_t counter = 20;

const uint16_t WATER_SENSE_PERIOD = 1000;
const uint16_t BUTTON_PUSH_PERIOD = 5000;//!!Important it is less than the ping time

//Enumeration of the different states
enum DeviceStates : int8_t
{
  POWEROFF,
  INIT,
  CONNECTING,
  OPERATING,
  FREEFALL,
  EMERGENCY,
  SAFE_DISCONNECT,
  UNSAFE_DISCONNECT
};