//Main.cpp 
//
// Author - Ree Surowiez
// Date - 09/07/21
#include <Arduino.h>
#include <ESP8266WiFi.h>

#define LATCH_PIN D5
#define BUTTON_PIN 
#define RED_LED 
#define YELLOW_LED
#define GREEN_LED

#ifdef DEBUG
#define DEBUG_LOGLN(a) Serial.println(a);
#define DEBUG_LOG(a) Serial.print(a);
#else
#define DEBUG_LOGLN(a) 
#define DEBUG_LOG(a) 
#endif

DeviceStates deviceState = DeviceStates.POWEROFF;
WiFiClient socketClient;
const char* ssid = "NA";
const char* password = "116028E44B";
const char* host = "192.168.1.100";
const uint16_t port = 11000;
const uint16_t networkTimeout = 10; //Timeout for connection to the network in seconds 
const uint16_t socketTimeout = 10; // Timeour for conncetion to the socket in seconds 


void setup() {
  
    //1. Latch Power on
    pinMode(LATCH_PIN, OUTPUT);
    digitalWrite(LATCH_PIN, HIGH);
    DEBUG_LOG("Power Latch successful");
    
    //2. Start Debug hardware checks
    #ifdef DEBUG
        Serial.begin(9600);
        debugChecks();
    #endif
    
    //3. Set the device state to connect to the socket
    deviceState = deviceStates.CONNECTING;
}

void loop() {

    switch(deviceState)
    {
        case POWEROFF:
            //1. Clean any resources and memory before shutdown
            #ifdef DEBUG 
                Serial.close();
            #endif
            //2. Disconnect the latch by pulling pin D5 low
            digitalWrite(LATCH_PIN, LOW);
        break;
        case CONNECTING:

            //1.Attempt connection to the network
            bool networkConnectStatus = deviceNetworkConnect(networkTimeout);
            if(networkConnectStatus == 0)
            {
                //if network connection failed then go to the safe disconnect state
                deviceState = DeviceStates.SAFE_DISCONNECT;
                break;
            }
            else
            {
                //if the network connection is successful then attempt connection to a socket
                bool socketConnectStatus = deviceSocketConnect();
                if (socketConnectStatus == 1) 
                {
                    //if the connection to the socket is successful then enter operating mode 
                    deviceState = DeviceStates.OPERATING;
                    break;
                }
                else
                {
                    deviceState = DeviceStates.SAFE_DISCONNECT; 
                    break;
                }
            }
        break;
        case OPERATING:

            //1. ping the socket
                //TODO: potentially use interrupts on a timer 
            //2. Sleep while still operating
                //TODO: Implement sleep feature
            //3. Read accelerometer 

            //4. Read button
                //TODO: implement circuit for reading the push button
            
            //5. Read battery level 
                //TODO: implement battery reading circuit

        break;
        case FALLING:
        break;
        case EMERGENCY:
        break;
        case SAFE_DISCONNECT:
        break;
        case UNSAFE_DISCONNECT:
        break;
    }
}

bool deviceSocketConnect()
{
  //Create wifi client and attempt connection to the specified socket
  client.setNoDelay(true);
  if (!client.connect(host, port)) {
    DEBUGLOGLN("Connection to server failed");
    //set program state to disconnecting
    return 0;
  }
  else
  {
    //if connected set program state to connected
    DEBUGLOGLN("Connection to server successful");
    return 1;
  }
}


bool deviceNetworkConnect(uint8_t networkTimeout)
{
  //attempt connection to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //diag messages
  DEBUG_LOGLN(" ");
  DEBUG_LOGLN("Connecting to network");

  //Wait for network connection
  for (uint8_t i = 0; i < 10; i++)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      //If not connected then print
      Serial.print(".");
    }
    else
    {
      //If connected then break out of loop
      break;
    }
    //Delay for 1/10 of timeout
    delay(networkTimeout * 100);
  }
  //Check network connection status
  if (WiFi.status() != WL_CONNECTED)
  {
    //if not connected
    DEBUGLOGLN("Network Connection Failed");
    //return 0 if failed
    return 0;
  }
  else
  {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.print(WiFi.localIP());
    return 1; 
  }
}




enum DeviceStates : int8_t
{
    POWEROFF, 
    CONNECTING, 
    OPERATING,
    FALLING, 
    EMERGENCY, 
    SAFE_DISCONNECT, 
    UNSAFE_DISCONNECT;
}

void deviceMonitor()
{
  //Set hardware
  setLED(false, false, true);

  //Check if client is created
  if (client == NULL)
  {
    DEBUGLOGLN("Error: Client Object doesn't exist");
    //Disconnect
    programState = disconnectingState;
    return;
  }
  else
  {
    while (true) {
      //If the client object exists
      if (client.connected())
      {
        setLED(false, true, true);
        DEBUGLOGLN("Sending PING");
        client.println("A");
        setLED(false, false, true);
      }
      else
      {
        DEBUGLOGLN("Error Client Disconnected");
        //Disconnect
        programState = disconnectingState;
        return;
      }
      for (int i = 0; i < 5; i++)
      {
        delay(1000);
        buttonState = digitalRead(POWERBUTTON);
        if (buttonState == HIGH)
        {
          programState = disconnectingState;
          return;
        }
      }
    }
  }
}

void deviceDisconnect()
{
  //set hardware
  setLED(false, false, false);
  //if socket is connected then send diconnect request to the server
  if (client.connected())
  {
    DEBUGLOGLN("Sending Disconnect Request");
    client.println("D");
    //Wait for disconnect acknowledge from server
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        DEBUGLOGLN("Timeout while waiting for disconnect confirmation");
        client.stop();
        break;
      }
    }

    if (client.connected())
    {
      while (client.available())
      {
        char ch = static_cast<char>(client.read());
        if (ch == 'D')
        {
          DEBUGLOGLN("Disconnect successful");
          break;
        }
        else
        {
          DEBUGLOGLN("Unknown disconnect");
          break;
        }
      }
    }
  }
    //Dispose of client
      if (client.connected()) {
        DEBUGLOGLN("Disconnecting from server");
        client.stop();
      }
    //Disconnect from access point
    if (WiFi.status() == WL_CONNECTED) {
      DEBUGLOGLN("Disconnecting from network");
      WiFi.disconnect();
    }

    //Wait for 3 seconds
    for (int i = 1; i < 4; i++)
    {
      setLED(false, true, false);
     delay(500);
      setLED(false, false, false);
      delay(500);
    }
    //Set to disconnected state
    programState = disconnectedState;
    return;

}


