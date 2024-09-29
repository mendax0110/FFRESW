/**
 * @file EthernetCommunication.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ETHERNET_COMMUNICATION_H
#define ETHERNET_COMMUNICATION_H

#include <Arduino.h>
#include <Ethernet.h>

/// @brief Class to handle Ethernet communication \class EthernetCommunication
class EthernetCommunication
{
public:
    EthernetCommunication();
    ~EthernetCommunication();

    void beginEthernet(byte* macAddress, IPAddress ip);
    void sendEthernetData(const char* data);
    void receiveEthernetData(char* buffer, size_t length);
    void handleEthernetClient();

private:
    EthernetServer* server;
    EthernetClient client;
    bool ethernetInitialized;
};

#endif // ETHERNET_COMMUNICATION_H
