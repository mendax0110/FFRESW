/**
 * @file EthernetCommunication.cpp
 * @brief Implementation of the Ethernet communication class.
 */

#include "ETHH.h"
#include <Ethernet.h>
#include <Arduino.h>

using namespace comModule;

EthernetCommunication::EthernetCommunication() : ethernetInitialized(false), server(80) {} // Initialize server with port 80

EthernetCommunication::~EthernetCommunication() {}

/// @brief Function to initialize the Ethernet communication
/// @param macAddress -> The MAC address to use for the Ethernet communication
/// @param ip -> The IP address to use for the Ethernet communication
void EthernetCommunication::beginEthernet(byte* macAddress, IPAddress ip)
{
    Ethernet.begin(macAddress, ip);
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware.");
        ethernetInitialized = false;
        return;
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
      ethernetInitialized = false;
      return;
    }
    server.begin();
    ethernetInitialized = true;
}

/// @brief Function to check if the Ethernet communication is initialized
/// @return -> true if the Ethernet communication is initialized, false otherwise
bool EthernetCommunication::isInitialized()
{
	return ethernetInitialized;
}

/// @brief Fucntion to send data over Ethernet
/// @param data -> The data to send
void EthernetCommunication::sendEthernetData(const char* data)
{
    if (!ethernetInitialized) return;

    if (client.connected())
    {
        client.println(data);
        client.stop();
    }
}

/// @brief Function to receive data over Ethernet
/// @param buffer -> The buffer to read the data into
/// @param length -> The length of the data to read
void EthernetCommunication::receiveEthernetData(char* buffer, size_t length)
{
    if (!ethernetInitialized) return;

    if (client.available())
    {
        size_t bytesRead = client.readBytes(buffer, length);
        buffer[bytesRead] = '\0';
    }
}

/// @brief Function to handle the Ethernet client
void EthernetCommunication::handleEthernetClient()
{
    EthernetClient newClient = server.available();
    if (newClient)
    {
        client = newClient;
        if (client.available())
        {
            char request[256];
            int bytesRead = client.readBytes(request, sizeof(request) - 1);
            request[bytesRead] = '\0';

            // Send response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Response data...");
        }
        client.stop();
    }
}