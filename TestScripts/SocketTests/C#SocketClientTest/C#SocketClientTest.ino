#include <ESP8266WiFi.h>

#define DEBUG
//Debug Serial log macros
#ifdef DEBUG
#define DEBUGLOG(str) (Serial.print(str))
#define DEBUGLOGLN(str) (Serial.println(str))
#else
#define DEBUGLOG(str)
#define DEBUGLOGLN(str)
#endif

//Pin defines
#define REDLED D1
#define YELLOWLED D2
#define GREENLED D3
#define POWERBUTTON D4

//State defines
#define disconnectedState 0
#define networkPendingState 1
#define socketPendingState 2
#define connectedState 3
#define disconnectingState 4


const char* ssid = "NA";
const char* password = "116028E44B";

WiFiClient client;

const char* host = "192.168.1.100";
const uint16_t port = 11000;
volatile bool buttonPushed;
uint8_t buttonState;
uint8_t programState;

//Function Prototypes
void deviceNetworkConnect();
void deviceSocketConnect();
void deviceMonitor();
void deviceDisconnect();

void setup() {
  //Serial Monitor Setup
  Serial.begin(9600);

  //Hardware setup
  pinMode(REDLED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(POWERBUTTON, INPUT);

  digitalWrite(REDLED, LOW);
  digitalWrite(YELLOWLED, LOW);
  digitalWrite(GREENLED, LOW);
  buttonPushed = false;
  //Set initial program state to disconnected
  programState = disconnectedState;
}

void loop() {
  switch (programState)
  {
    case disconnectedState:
      deviceDisconnected();
      break;
    case networkPendingState:
      deviceNetworkConnect(10);
      break;
    case socketPendingState:
      deviceSocketConnect();
      break;
    case connectedState:
      deviceMonitor();
      break;
    case disconnectingState:
      deviceDisconnect();
      break;
  }
}

void deviceDisconnected()
{
  //Set hardware
  setLED(true, false, false);
  //Sleep

  //Wait for button push
  buttonState = digitalRead(POWERBUTTON);
  while (buttonState == LOW) {
    delay(200);
    buttonState = digitalRead(POWERBUTTON);
  }
  //Change state
  programState = networkPendingState;
}

void deviceNetworkConnect(uint8_t timeout)
{
  //Set hardware
  setLED(false, true, false);

  //attempt connection to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //diag messages
  DEBUGLOGLN(" ");
  DEBUGLOGLN("Connecting to network");

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
    delay(timeout * 100);
  }
  //Check network connection status
  if (WiFi.status() != WL_CONNECTED)
  {
    //if not connected
    DEBUGLOGLN("Network Connection Failed");
    //switch to disconnecting state
    programState = disconnectingState;
  }
  else
  {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //Set state to socket connect
    programState = socketPendingState;
  }
}
void deviceSocketConnect()
{
  //Create wifi client and attempt connection to the specified socket
  client.setNoDelay(true);
  if (!client.connect(host, port)) {
    DEBUGLOGLN("Connection to server failed");
    //set program state to disconnecting
    programState = disconnectingState;
    return;
  }
  else
  {
    //if connected set program state to connected
    DEBUGLOGLN("Connection to server successful");
    programState = connectedState;
    return;
  }
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


void setLED(bool red, bool yellow, bool green)
{
  digitalWrite(REDLED, red);
  digitalWrite(YELLOWLED, yellow);
  digitalWrite(GREENLED, green);
}
