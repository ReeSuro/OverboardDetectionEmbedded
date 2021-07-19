//NetworkManager.cpp
//
//Author: Ree Surowiez
//Date: 15/07/21

#include "NetworkManager.hpp"

bool NetworkManager::socketConnect(const char *host, const uint16_t port)
{
    //Create wifi client and attempt connection to the specified socket
    client.setNoDelay(true);
    if (!client.connect(host, port))
    {
        DEBUG_LOGLN("Connection to server failed");
        //set program state to disconnecting
        return 0;
    }
    else
    {
        //if connected set program state to connected
        DEBUG_LOGLN("Connection to server successful");
        return 1;
    }
}

bool NetworkManager::networkConnect(uint8_t networkTimeout, const char *ssid, const char *password)
{
    //attempt connection to network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    //diag messages
    DEBUG_LOGLN(" ")
    DEBUG_LOGLN("Connecting to network")

    //Wait for network connection
    for (uint8_t i = 0; i < 10; i++)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            //If not connected then print
            DEBUG_LOGLN(".");
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
        DEBUG_LOGLN("Network Connection Failed");
        //return 0 if failed
        return 0;
    }
    else
    {
        DEBUG_LOGLN("\nWiFi connected");
        DEBUG_LOGLN("IP address: ");
        DEBUG_LOGLN(WiFi.localIP());
        return 1;
    }
}

bool NetworkManager::sendPing(const char *message)
{
    //Check if client is created
    if (client == NULL || !client.connected())
    {
        DEBUG_LOGLN("Error: Client Object doesn't exist");
        return 0;
    }
    else
    {
        //If the client object exists
        DEBUG_LOGLN("Sending PING");
        client.println(message);
        return 1;
    }
}

