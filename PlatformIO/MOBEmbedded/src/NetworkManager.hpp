#pragma once
#include <ESP8266WiFi.h>
#include "Debug.hpp"

struct NetworkSettings
{

public:
    NetworkSettings(const char *ssid, const char *password, const char *host, const uint16_t port, const uint16_t networkTimeout,
                    const uint16_t socketTimeout, const uint16_t pingTime, String pingMessage) : ssid(ssid), password(password), host(host), port(port), networkTimeout(networkTimeout), socketTimeout(socketTimeout), pingTime(pingTime), pingMessage(pingMessage)
    {
    }

    const char *ssid;
    const char *password;
    const char *host;
    const uint16_t port;
    const uint16_t networkTimeout; //Timeout for connection to the network in seconds
    const uint16_t socketTimeout;  // Timeour for conncetion to the socket in seconds
    const uint16_t pingTime;
    String pingMessage;
};

class NetworkManager
{
public:
    NetworkManager(NetworkSettings settings);
    bool networkConnect();
    bool socketConnect();
    bool sendPing(const char *message);
    bool safeDisconnect();
    bool dispose();
    NetworkSettings settings;

private:
    WiFiClient client;
};