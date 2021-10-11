#include "MOBDevice.hpp"

MOBDevice::MOBDevice(MPU6050 *acc1, BatteryManager *bat1, LEDManager *led1, WaterSensor *ws1, NetworkManager *nm1,
                     const uint8_t latch, const uint8_t button, const uint16_t waterSensePeriod1,
                     const uint16_t buttonPressPeriod1, const uint16_t fallThresholdTime1) : acc(acc1),
                                                                                             bat(bat1),
                                                                                             led(led1),
                                                                                             ws(ws1),
                                                                                             nm(nm1),
                                                                                             latchPin(latch),
                                                                                             buttonPin(button),
                                                                                             waterSensePeriod(waterSensePeriod1),
                                                                                             buttonPressPeriod(buttonPressPeriod1),
                                                                                             fallThresholdTime(fallThresholdTime1)
{
    deviceState = DeviceStates::POWEROFF;

    pinMode(latch, OUTPUT);
    pinMode(button, INPUT);
}

void MOBDevice::start()
{
    //1. Latch the device on
    digitalWrite(latchPin, HIGH);
    //2. Set the device state and enter the state loop
    deviceState = DeviceStates::INIT;

    while (true)
    {
        switch (deviceState)
        {
        case POWEROFF:
        {
            //Blink leds
            led->blinkRed(1000);
            led->blinkRed(1000);
            led->blinkRed(1000);

            //unlatch the device
            digitalWrite(latchPin, LOW);
            break;
        }
        case INIT:
        {
            //Start interrupt for the accelerometer
            acc->begin();
            deviceState = DeviceStates::CONNECTING;
            break;
        }
        case CONNECTING:
        {
            led->greenOn();
            //1. Attempt connection to the network
            Serial.println("Attempting network connection");
            bool networkConnectSuccess = nm->networkConnect();
            if (!networkConnectSuccess)
            {
                //If the network connection failed then go to safe disconnect
                Serial.println("Network connection failed");
                deviceState = DeviceStates::UNSAFE_DISCONNECT;
                break;
            }
            else
            {
                //If the connection was successful then attempt connection to the socket
                Serial.println("Network connection Success");
                bool socketConnectStatus = nm->socketConnect();
                if (!socketConnectStatus)
                {
                    Serial.println("Socket connection Failed");
                    deviceState = DeviceStates::UNSAFE_DISCONNECT;
                }
                else
                {
                    //if socket connection is successful then move to the operating state
                    Serial.println("Socket connection Successful");
                    deviceState = DeviceStates::OPERATING;
                }
            }
            led->greenOff();
            break;
        }
        case OPERATING:
        {
            //1. Read battery level and determine if the required battery level is above the required voltage
            led->greenOn();
            bool batteryState = bat->isVoltageSafe();
            //if (!batteryState)
            //{
            //if the battery voltage has gone below a selected threshold then turn off
            //    Serial.println("Battery voltage too low, disconnecting");
            //    deviceState = DeviceStates::SAFE_DISCONNECT;
            //    break;
            //}

            //2. ping the socket
            String voltage = bat->getVoltageAsString();
            String message = nm->settings.pingMessage + voltage;
            //Calculate checksum of the message
            char cMessage[message.length() + 1];
            strcpy(cMessage, message.c_str());
            int sum = 0;
            for (int i = 0; i < (int)message.length(); i++)
            {
                sum += cMessage[i];
            }
            message += "#" + String(sum) + "<EOF>";
            if (deviceState == DeviceStates::FALLING_ANALYSIS)
                break;
            nm->sendPing(message.c_str());
            Serial.println(message);
            led->greenOff();
            //3. Sleep while still operating : TODO IMPLEMENT SLEEP
            if (deviceState == DeviceStates::FALLING_ANALYSIS)
                break;
            delay(nm->settings.pingTime);
            break;
        }
        case FALLING_ANALYSIS:
        {
            //Read water sensor for a set period of time
            bool success = false;
            for (uint8_t i = 0; i < 10; i++)
            {
                Serial.println("Searching for water");
                if (ws->isWet())
                {
                    //if water is detected then enter emergency mode
                    success = true;
                    break;
                }
                else
                {
                    delay(waterSensePeriod / 10);
                }
            }
            if (success)
            {
                deviceState = DeviceStates::EMERGENCY;
            }
            else
            {
                deviceState = DeviceStates::OPERATING;
            }
            break;
        }
        case EMERGENCY:
        {
            led->greenOn();
            led->redOn();
            //Start the strobe LED
            led->strobe(1000);
            break;
        }
        case SAFE_DISCONNECT:
        {
            //Disconnect from the server and dispose of resources
            led->greenOff();
            led->redOn();
            bool success = false;
            for (int i = 0; i < 5; i++)
            {
                Serial.println("Attempting Disconnect");
                bool result = nm->safeDisconnect();
                if (result)
                {
                    Serial.println("Disconnect Acknowledged");
                    nm->dispose();
                    deviceState = DeviceStates::POWEROFF;
                    success = true;
                    Serial.println("Disconnect Successful");
                    break;
                }
                else
                {
                    delay(1000);
                }
            }
            if (!success) //If not successful then return to the operating state
            {
                deviceState = DeviceStates::OPERATING;
            }
            led->redOff();
        }
        case UNSAFE_DISCONNECT:
        {
            led->greenOff();
            led->redOn();
            //Dispose of resources
            Serial.println("Disposing of resources");
            nm->dispose();
            deviceState = DeviceStates::POWEROFF;
            while (!digitalRead(buttonPin))
                ; //Make sure off button still isnt held down
            led->greenOff();
            led->redOff();
            break;
        }
        }
    }
}

void MOBDevice::falling()
{
    //Triggered when the device is falling
    //1. Wait until the device is not falling
    //Start timer
    Serial.println("Falling");
      if (deviceState == DeviceStates::EMERGENCY)
    {
        return;
    }
    unsigned long currentTime = millis();
    unsigned long nextTime = currentTime + fallThresholdTime;
    //while (acc->isFalling()); TODO:REMOVE COMMENT ON RELEASE
    while (!digitalRead(D6))
        ;
    //end timer
    currentTime = millis();
    if (currentTime > nextTime)
    {
        deviceState = DeviceStates::FALLING_ANALYSIS;
    }
}

void MOBDevice::buttonPress()
{
    Serial.println("Button Pressed");
    //Check if button pressed while connecting
    if (deviceState == DeviceStates::CONNECTING)
    {
        deviceState = DeviceStates::POWEROFF;
        while (!digitalRead(buttonPin))
            ;
        return;
    }
    //1. Time button press
    //Start timer
    unsigned long currentTime = millis();
    unsigned long nextTime = currentTime + buttonPressPeriod;
    //Wait for the button to be release
    while (!digitalRead(buttonPin))
    {
        //end timer
        currentTime = millis();
        if (currentTime > nextTime)
        {
            //If the timer is less than X seconds then return else determine the state

            if (deviceState == DeviceStates::EMERGENCY)
            {
                deviceState = DeviceStates::UNSAFE_DISCONNECT;
            }
            else if (deviceState == DeviceStates::OPERATING)
            {
                deviceState = DeviceStates::SAFE_DISCONNECT;
            }
            break;
        }
    }
}

void MOBDevice::debugStart()
{
    switch (deviceState)
    {
    case POWEROFF:
    {
        //1. Clean any resources and memory before shutdown
        Serial.flush();
        Serial.end();
        //2. blink lights
        //Blink leds
        led->blinkRed(1000);
        led->blinkRed(1000);
        led->blinkRed(1000);

        //3. Disconnect the latch by pulling pin D5 low
        digitalWrite(latchPin, LOW);
        break;
    }
    case INIT:
    {
        //Initialise sensors
        bool result = debugInitDevice();
        if (result)
            deviceState = DeviceStates::CONNECTING;
        else
            deviceState = DeviceStates::SAFE_DISCONNECT;
    }
    case CONNECTING:
    {
        //1. Attempt connection to the network
        bool networkConnectSuccess = nm->networkConnect();
        if (!networkConnectSuccess)
        {
            //If the network connection failed then go to safe disconnect
            deviceState = DeviceStates::SAFE_DISCONNECT;
            break;
        }
        else
        {
            //If the connection was successful then attempt connection to the socket
            bool socketConnectStatus = nm->socketConnect();
            if (!socketConnectStatus)
            {
                deviceState = DeviceStates::SAFE_DISCONNECT;
            }
            else
            {
                //if socket connection is successful then move to the operating state
                deviceState = DeviceStates::OPERATING;
            }
        }
        break;
    }
    case OPERATING:
    {
        //1. Read battery level and determine if the required battery level is above the required voltage
        bool batteryState = bat->isVoltageSafe();
        if (!batteryState)
        {
            //if the battery voltage has gone below a selected threshold then turn off
            deviceState = DeviceStates::SAFE_DISCONNECT;
            break;
        }

        //2. ping the socket
        String voltage = bat->getVoltageAsString();
        String message = voltage + " " + nm->settings.pingMessage;
        nm->sendPing(message.c_str());
        //2. Sleep while still operating
        delay(nm->settings.pingTime);
        break;
    }
    case EMERGENCY:
    {
        //1. Start the strobe led
        led->strobe(1000);
        break;
    }
    case SAFE_DISCONNECT:
    {
        //1. Disconnect from the socket with confirmation from the server

        //2. Disconnect from the network

        //3. Dispose of network objects

        //4. Dispose of hardware objects
        break;
    }
    case UNSAFE_DISCONNECT:
    {
        //1. Dispose of network objects

        //2, Dispose of hardware objects
        break;
    }
    }
}

bool MOBDevice::debugInitDevice()
{
    bool debugSuccessful = true;
    Serial.println("--------------------- Beginning Debug Checks ----------------------");
    Serial.println("Setting up MPU6050 - ");

    //Initialise and check the accelerometer
    Serial.println("Establishing communication: ");
    bool acc_error = acc->exists();
    Serial.println((acc_error ? "SUCCESS" : "FAILED"));
    //if communication is possible then continue setup
    if (acc_error)
    {
        Serial.println("Setting up accelerometer - ");
        acc->begin(); //Set up interrupts
        uint8_t *acc_regs = acc->getSettings();
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

    Serial.println("Testing LEDS - ");
    Serial.println("RED LED ON: NOW ");
    led->blinkRed(2000);
    Serial.println("GREEN LED ON: NOW ");
    led->blinkGreen(2000);
    Serial.println("All LEDs ON: NOW ");
    led->redOn();
    led->greenOn();
    delay(2000);
    led->redOff();
    led->greenOff();

    //Test water sensor
    Serial.println("Testing Water Sensor - ");

    if (ws->isWet())
    {
        Serial.println("Please seperate water sensor terminals:");
        while (ws->isWet())
            ;
        delay(500);
    }
    Serial.println("Contact water sense terminals:");
    bool contacted = false;
    for (uint8_t i = 0; i < 20; i++)
    {
        if (ws->isWet() == 0)
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

    //Battery sensor tests
    Serial.println("Setting up Battery Sensor - ");
    if (bat->isVoltageSafe())
    {
        Serial.println("Battery IS IN A SAFE VOLTAGE RANGE");
    }
    else
    {
        Serial.println("Battery IS IN A UNSAFE VOLTAGE RANGE");
    }
    Serial.print("Battery READING : ");
    Serial.println(bat->getVoltageAsString());
    Serial.println("BATTERY DEBUG COMPLETE");

    if (debugSuccessful)
    {
        Serial.println("Debug checks successful");
        return 1;
    }
    else
    {
        Serial.println("Debug checks failed");
        return 0;
    }
}
