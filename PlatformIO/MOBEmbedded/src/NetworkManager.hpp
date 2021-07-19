#include <ESP8266WiFi.h>
#include "Debug.hpp"

class NetworkManager 
{
    public:
   
        bool networkConnect(uint8_t networkTimeout, const char *ssid, const char *password);
        bool socketConnect(const char* host, const uint16_t port);
        bool sendPing(const char* message);

    private:
        WiFiClient client;



};