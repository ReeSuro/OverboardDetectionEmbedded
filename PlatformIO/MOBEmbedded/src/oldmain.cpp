// //Main.cpp
// //
// // Author - Ree Surowiez
// // Date - 09/07/21
// #include <Arduino.h>
// #define DEBUG
// #define UNIT_TESTS
// #include "main.hpp"
// #include "Debug.hpp"
// #include "MPU6050.hpp"
// #include "Settings.hpp"
// #include "NetworkManager.hpp"
// #include "LEDManager.hpp"

// #ifdef UNIT_TESTS

//   void setup()
//   {
//       Serial.begin(9600);
//       //Test LEDManager
//       LEDManager leds(RED_LED, GREEN_LED, STROBE_LED);
//       //Test the constructor
//       if(leds.getRedPin() == RED_LED)
//         Serial.println("RED LED PIN CORRECT");
//       else
//         Serial.println("RED LED PIN INCORRECT");

//       if(leds.getGreenPin() == GREEN_LED)
//         Serial.println("GREEN LED PIN CORRECT");
//       else
//         Serial.println("GREEN LED PIN INCORRECT");

//       if(leds.getStrobePin() == STROBE_LED)
//         Serial.println("STROBE LED PIN CORRECT");
//       else
//         Serial.println("STROBE LED PIN INCORRECT");  

//       if(digitalRead(RED_LED) == 0)
//         Serial.println("RED LED PIN SET CORRECT");
//       else
//         Serial.println("RED LED PIN SET INCORRECT");

//       if(digitalRead(GREEN_LED) == 0)
//         Serial.println("GREEN LED PIN SET CORRECT");
//       else
//         Serial.println("GREEN LED PIN SET INCORRECT");

//       if(digitalRead(STROBE_LED) == 0)
//         Serial.println("STROBE LED PIN SET CORRECT");
//       else
//         Serial.println("STROBE LED PIN SET INCORRECT");

//   }

//   void loop()
//   {
//       delay(10000);
      
//   }



// #else


// DeviceStates deviceState = DeviceStates::POWEROFF;
// MPU6050 accelerometer;
// NetworkManager network;
// //WaterSensor ws;
// //BatteryManager battery;
// LEDManager leds(RED_LED, GREEN_LED, STROBE_LED);




// void setup()
// {
//   //Latch Power on
//   pinMode(LATCH_PIN, OUTPUT);
//   digitalWrite(LATCH_PIN, HIGH);
//   DEBUG_LOGLN("Power Latch successful");
//   deviceState = DeviceStates::INIT;
// //2. Start serial
// #ifdef DEBUG
//   Serial.begin(9600);
// #endif
// }

// void loop()
// {

//   switch (deviceState)
//   {
//   case POWEROFF:
//   {

// //1. Clean any resources and memory before shutdown
// #ifdef DEBUG
//     Serial.flush();
//     Serial.end();
// #endif
//     //2. blink lights
//     SetLEDS(true, false, false);
//     delay(1000);
//     SetLEDS(false, false, false);
//     delay(1000);
//     SetLEDS(true, false, false);
//     delay(1000);
//     SetLEDS(false, false, false);
//     //3. Disconnect the latch by pulling pin D5 low
//     digitalWrite(LATCH_PIN, LOW);
//     break;
//   }
//   case INIT:
//   {
//     //Initialise sensors
//     bool result = initDevice(accelerometer);
//     if (result)
//       deviceState = DeviceStates::POWEROFF;
//     else
//       deviceState = DeviceStates::POWEROFF;
//   }
//   case CONNECTING:
//   {
//     //1. Attempt connection to the network
//     bool networkConnectSuccess = network.networkConnect(networkTimeout, ssid, password);
//     if (!networkConnectSuccess)
//     {
//       //If the network connection failed then go to safe disconnect
//       deviceState = DeviceStates::SAFE_DISCONNECT;
//       break;
//     }
//     else
//     {
//       //If the connection was successful then attempt connection to the socket
//       bool socketConnectStatus = network.socketConnect(socketTimeout,host, port);
//       if (!socketConnectStatus)
//       {
//         deviceState = DeviceStates::SAFE_DISCONNECT;
//       }
//       else
//       {
//         //if socket connection is successful then move to the operating state
//         deviceState = DeviceStates::OPERATING;
//       }
//     }
//     break;
//   }
//   case OPERATING:
//   {
//     //1. Read battery level and determine if the required battery level is above the required voltage
//     bool batteryState = battery.isVoltageSafe();
//     if (!batteryState)
//     {
//       //if the battery voltage has gone below a selected threshold then turn off
//       deviceState = DeviceStates::SAFE_DISCONNECT;
//       break;
//     }

//     //2. ping the socket
//     String voltage = battery.getVoltageAsString();
//     String message = voltage + " " + pingMessage;
//     network.sendPing(message);
//     //2. Sleep while still operating
//     delay(pingTime);
//     break;
//   }
//   case EMERGENCY:
//   {
//     //1. Start the strobe led
//     leds.strobe();
//     break;
//   }
//   case SAFE_DISCONNECT:
//   {
//     //1. Disconnect from the socket with confirmation from the server

//     //2. Disconnect from the network

//     //3. Dispose of network objects

//     //4. Dispose of hardware objects
//     break;
//   }
//   case UNSAFE_DISCONNECT:
//   {
//     //1. Dispose of network objects

//     //2, Dispose of hardware objects
//     break;
//   }
//   }
// }

// #pragma region ISRs
// IRAM_ATTR void onFalling()
// {
//   //Triggered when the device is falling
//   //1. Wait until the device is not falling, TODO: ADD TIMER to measure fall time.
//   while (accelerometer.isFalling())
//     ;

//   //2.  Read water sensor for a set period of time
//   for (uint8_t i = 0; i < 10; i++)
//   {
//     if (!digitalRead(WATER_SENSE_PIN))
//     {
//       //if water is detected then enter emergency mode
//       deviceState = DeviceStates::EMERGENCY;
//       break;
//     }
//     else
//     {
//       delay(WATER_SENSE_PERIOD / 10);
//     }
//   }
// }
// IRAM_ATTR void onButton()
// {

//   //1. Time button press
//   //Start timer
//   unsigned long currentTime = millis();
//   unsigned long nextTime = currentTime + BUTTON_PUSH_PERIOD;
//   //Wait for the button to be release
//   while (!digitalRead(BUTTON_INT_PIN))
//     ;
//   //end timer
//   currentTime = millis();
//   if (currentTime > nextTime)
//   {
//     //If the timer is less than X seconds then return else determine the state

//     if (deviceState == DeviceStates::EMERGENCY)
//     {
//       deviceState = DeviceStates::UNSAFE_DISCONNECT;
//     }
//     else if (deviceState == DeviceStates::OPERATING)
//     {
//       deviceState = DeviceStates::SAFE_DISCONNECT;
//     }
//   }
// }
// #pragma endregion




// bool initDevice(MPU6050 accelerometer)
// {

// #ifdef DEBUG
//   bool debugSuccessful = true;
//   Serial.println("--------------------- Beginning Debug Checks ----------------------");
//   Serial.println("Setting up MPU6050 - ");

//   //Initialise and check the accelerometer
//   Serial.println("Establishing communication: ");
//   bool acc_error = accelerometer.exists();
//   Serial.println((acc_error ? "SUCCESS" : "FAILED"));
//   //if communication is possible then continue setup
//   if (acc_error)
//   {
//     Serial.println("Setting up accelerometer - ");
//     accelerometer.begin(threshold, counter); //Set up interrupts
//     uint8_t *acc_regs = accelerometer.getSettings();
//     Serial.print("POWER REGISTER 1  :");
//     Serial.println(acc_regs[0], BIN);
//     Serial.print("SIGNAL PATH RESET :");
//     Serial.println(acc_regs[1], BIN);
//     Serial.print("INT PIN REGISTER  :");
//     Serial.println(acc_regs[2], BIN);
//     Serial.print("HPF CONFIG REG    :");
//     Serial.println(acc_regs[3], BIN);
//     Serial.print("THRESHOLD REGISTER:");
//     Serial.println(acc_regs[4], BIN);
//     Serial.print("COUNTER REGISTER  :");
//     Serial.println(acc_regs[5], BIN);
//     Serial.print("DECREMENT REGISTER:");
//     Serial.println(acc_regs[6], BIN);
//     Serial.print("INT ENABLE REG    :");
//     Serial.println(acc_regs[7], BIN);
//     Serial.println("");
//     free(acc_regs);
//     acc_regs = NULL;
//   }
//   else
//   {
//     debugSuccessful = false;
//   }
//   delay(1000);
// #else
//   accelerometer.begin(threshold, counter); //Set up free fall interrupt
// #endif

// #ifdef DEBUG
//   Serial.println("Testing LEDS - ");
//   pinMode(RED_LED, OUTPUT);
//   pinMode(GREEN_LED, OUTPUT);
//   SetLEDS(false, false, false);
//   Serial.println("RED LED ON: NOW ");
//   digitalWrite(RED_LED, HIGH);
//   delay(2000);
//   digitalWrite(RED_LED, LOW);
//   Serial.println("GREEN LED ON: NOW ");
//   digitalWrite(GREEN_LED, HIGH);
//   delay(2000);
//   digitalWrite(GREEN_LED, LOW);
//   Serial.println("All LEDs ON: NOW ");
//   SetLEDS(true, true, true);
//   delay(2000);
//   SetLEDS(true, false, false);
//   pinMode(BUTTON_INT_PIN, INPUT);
// #else
//   pinMode(BUTTON_INT_PIN, INPUT);
//   pinMode(RED_LED, OUTPUT);
//   pinMode(GREEN_LED, OUTPUT);
//   SetLEDS(true, false, false);
// #endif
// //Test water sensor
// #ifdef DEBUG
//   Serial.println("Testing Water Sensor - ");

//   if (digitalRead(WATER_SENSE_PIN) == 0)
//   {
//     Serial.println("Please seperate water sensor terminals:");
//     while (digitalRead(WATER_SENSE_PIN) == 0)
//       ;
//     delay(500);
//   }
//   Serial.println("Contact water sense terminals:");
//   bool contacted = false;
//   for (uint8_t i = 0; i < 20; i++)
//   {
//     if (digitalRead(WATER_SENSE_PIN) == 0)
//     {
//       contacted = true;
//       break;
//     }
//     delay(500);
//   }
//   Serial.print("WATER SENSOR SETUP: ");
//   Serial.println(contacted ? "SUCCESS" : "FAILED");
//   if (!contacted)
//   {
//     debugSuccessful = false;
//   }
// #endif
//   //TODO: ADD CHECKS FOR BATTERY VOLTAGE SENSOR

//   //set up interrupts
//   attachInterrupt(digitalPinToInterrupt(ACC_INT_PIN), onFalling, FALLING);
//   attachInterrupt(digitalPinToInterrupt(BUTTON_INT_PIN), onButton, FALLING);

// #ifdef DEBUG
//   if (debugSuccessful)
//   {
//     Serial.println("Debug checks successful");
//     return 1;
//   }
//   else
//   {
//     Serial.println("Debug checks failed");
//     return 0;
//   }
// #else
//   pinMode(WATER_SENSE_PIN, INPUT);
//   return 1;
// #endif
// }

// #endif
