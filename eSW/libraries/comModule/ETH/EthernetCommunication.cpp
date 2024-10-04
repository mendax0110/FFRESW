/**
 * @file EthernetCommunication.cpp
 * @brief Implementation of the Ethernet communication class.
 */

#include "EthernetCommunication.h"

EthernetCommunication::EthernetCommunication() : ethernetInitialized(false), server(nullptr) {}

EthernetCommunication::~EthernetCommunication()
{
    if (server)
    {
        delete server;
    }
}

/// @brief Function to initialize the Ethernet communication
/// @param macAddress -> The MAC address to use for the Ethernet communication
/// @param ip -> The IP address to use for the Ethernet communication
void EthernetCommunication::beginEthernet(byte* macAddress, IPAddress ip)
{
    Ethernet.begin(macAddress, ip);
    server = new EthernetServer(80);
    server->begin();
    ethernetInitialized = true;
}

/// @brief Fucntion to send data over Ethernet
/// @param data -> The data to send
void EthernetCommunication::sendEthernetData(const char* data)
{
    if (!ethernetInitialized) return;

    client = server->available();
    if (client)
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

    client = server->available();
    if (client)
    {
        size_t bytesRead = client.readBytes(buffer, length);
        buffer[bytesRead] = '\0';
    }
}

/// @brief Function to handle the Ethernet client
void EthernetCommunication::handleEthernetClient()
{
    if (client)
    {
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
