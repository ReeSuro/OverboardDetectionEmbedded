#include <ESP8266WiFi.h>
#include "Debug.hpp"

class NetworkManager 
{
    public:
        NetworkManager();
        bool NetworkManager::deviceNetworkConnect(uint8_t networkTimeout, const char *ssid, const char *password);
        bool NetworkManager::socketConnect(const char* host, const uint16_t port);
        bool NetworkManager::sendPing(const char* message);

    private:
        WiFiClient client;



};