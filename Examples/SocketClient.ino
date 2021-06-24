/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

#define redLED D1
#define yellowLED D2
#define greenLED D3
#define onButton D4

const char* ssid = "NA";
const char* password = "116028E44B";

const char* host = "192.168.1.1";
const uint16_t port = 11000;
volatile bool buttonPushed;
uint8_t buttonState;

bool deviceNetworkConnect(uint8_t timeout);
void deviceServerConnect();


void setup() {
  //Serial Monitor Setup
  Serial.begin(9600);
  
  //Hardware setup
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(onButton, INPUT);
  
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);
  buttonPushed = false;
}

void loop() {

 //Set state LED to disconnected
  digitalWrite(redLED, HIGH);
  //Wait for button push
  buttonState = digitalRead(onButton);
  Serial.println(buttonState);
  while(buttonState == LOW){
    delay(200);
    buttonState = digitalRead(onButton);
  }
  
  //If pushed attempt to connect to network
   //Set connecting LED
   buttonPushed = true;
  digitalWrite(yellowLED, HIGH);
  digitalWrite(redLED, LOW);

  //Attempt to connect to the network
  if(deviceNetworkConnect(10))
  {
    //If connection to network is successful attempt to communicate with the server
    while(true)
    {
      Serial.println("Attempting connect to server");
      deviceServerConnect();
      delay(5000);
      if(buttonPushed == false)
      {
        digitalWrite(greenLED, LOW);
        break;
      }  
    }
  }
  else 
  {
    //If connection failed reset loop
    digitalWrite(yellowLED, LOW);
  }
}

void deviceServerConnect()
{
  // Use WiFiClient class to create TCP connections
   WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to server failed");
    return;
  }
  //Set connected led
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, HIGH);
  
  //Loop until button is pushed
  while(buttonPushed)
  {
    //Send a ping every 5 seconds
     Serial.println("sending ping");
     if (client.connected()) {
       client.println("A");
     }
     else
     {
      Serial.println("Error server disconnected");
      break; 
     }
     for(int i = 0; i < 5; i++)
     {
        delay(1000);
        buttonState = digitalRead(onButton);
        if(buttonState == HIGH)
        {
          buttonPushed = false;
          break;
        }  
     }
  }

  //Disconnect from socket
  //Send disconnect request
  if (client.connected()) 
  {
     client.println("D");
     //Wait for disconnect acknowledge from server
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }
      Serial.println("Receiving disconnect acknowledge from the server");
      while (client.available()) {
      char ch = static_cast<char>(client.read());

      if(ch == 'D')
      {
        Serial.println("Disconnect successful");  
      }
      else
      {
        Serial.println("Unknown disconnect");  
      }
       // Close the connection
       Serial.println();
       Serial.println("closing connection");
      client.stop();
      return; 
  }
 } 
}
  



bool deviceNetworkConnect(uint8_t timeout)
{
  digitalWrite(yellowLED, HIGH);
  //Attempt a connection
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //Set node to client and connect to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  
  for(uint8_t i = 0; i < 10; i++) 
  {
      if(WiFi.status() != WL_CONNECTED)
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
      delay(timeout*100);
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    //If not connected
    Serial.println("Connection Failed");  
    return false;
  }  
  else
  {
     Serial.println("");
     Serial.println("WiFi connected");
     Serial.println("IP address: ");
     Serial.println(WiFi.localIP());
     return true;
  }
}
