/**
 * @file EthernetCommunication.cpp
 * @brief Implementation of the Ethernet communication class.
 */
#include "ETHH.h"
#include <Ethernet.h>
#include <Arduino.h>

using namespace comModule;

EthernetCommunication::EthernetCommunication() : ethernetInitialized(false), server(80) {}

EthernetCommunication::~EthernetCommunication() {}

/// @brief Function to initialize the Ethernet communication
/// @param macAddress -> The MAC address to use for the Ethernet communication
/// @param ip -> The IP address to use for the Ethernet communication
void EthernetCommunication::beginEthernet(uint8_t* macAddress, IPAddress ip)
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
    Serial.print("Client is at: ");
    Serial.println(Ethernet.localIP());
}

/// @brief Function to check if the Ethernet communication is initialized
/// @return -> true if the Ethernet communication is initialized, false otherwise
bool EthernetCommunication::isInitialized()
{
	return ethernetInitialized;
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

        if (client.connected())
        {
            char request[256] = {0};
            int bytesRead = client.readBytesUntil('\n', request, sizeof(request) - 1);

            if (bytesRead > 0)
            {
                client.println(F("HTTP/1.1 200 OK"));
                client.println(F("Content-Type: application/json"));
                client.println(F("Connection: close"));
                client.println(); // End of headers
            }
        }
        client.stop();
    }
}

/// @brief Fucntion to send data over Ethernet
/// @param endpoint -> endpoint to send data to
/// @param data -> The data to send
void EthernetCommunication::sendEthernetData(const char* endpoint, const char* data)
{
    if (!ethernetInitialized) return;

    EthernetClient activeClient = server.available();
    if (activeClient.connected())
    {
        // Send HTTP header
        activeClient.println("POST " + String(endpoint) + " HTTP/1.1");
        activeClient.println("Host: ESW");
        activeClient.println("Content-Type: application/json");
        activeClient.println("Connection: close");

        // Send JSON data
        activeClient.println("Content-Length: " + String(strlen(data)));
        activeClient.println();
        activeClient.println(data);

        activeClient.stop();
    }
}

/// @brief Function to get the current status of the flag
/// @return sendDataFlag -> true if data should be sent, false otherwise
bool EthernetCommunication::getSendDataFlag() const
{
	return sendDataFlag;
}

/// @brief Function to get the current status of the flag
/// @param flag -> set the flag to true if data sent, false otherwise
void EthernetCommunication::setSendDataFlag(bool flag)
{
	sendDataFlag = flag;
}

/// @brief Function to handle set the compound command for the valve uC Slave
/// @param id -> id of the command
/// @param index -> actual index of the command
/// @param value -> value of the command
void EthernetCommunication::setCompound(ID id, int index, String value)
{
    if (!ethernetInitialized) return;

    String command;
    switch (id)
    {
        case ID::ACCESS_MODE:
            command = "p:28xx0A0200000F0B0000";
            break;
        case ID::CONTROL_MODE:
            command = "p:28xx0A0200010F020000";
            break;
        case ID::ACTUAL_POSITION:
            command = "p:28xx0A02000210010000";
            break;
        case ID::POSITION_STATE:
            command = "p:28xx0A02000310100000";
            break;
        case ID::ACTUAL_PRESSURE:
            command = "p:28xx0A02000407010000";
            break;
        case ID::TARGET_PRESSURE_USED:
            command = "p:28xx0A02000507030000";
            break;
        case ID::WARNING_BITMAP:
            command = "p:28xx0A0200060F300100";
            break;
        default:
            return;
    }

    command += String(";") + String(index) + String(";") + value;

    EthernetClient activeClient = server.available();
    if (activeClient.connected())
    {
        activeClient.println(command);
        activeClient.stop();
    }
}

/// @brief Function to handle get the compound command for the valve uC Slave
/// @param id -> id of the command
/// @param index -> actual index of the command
/// @return response -> string with the response of valve uC slave
String EthernetCommunication::getCompound(ID id, int index = 0)
{
    if (!ethernetInitialized) return "[ERROR] ethernetshield not init";

    String command;
    String response;

    switch (id)
    {
        case ID::ACCESS_MODE:
            command = "p:29xx0A0200000F0B0000";
            break;
        case ID::CONTROL_MODE:
            command = "p:29xx0A0200010F020000";
            break;
        case ID::ACTUAL_POSITION:
            command = "p:29xx0A02000210010000";
            break;
        case ID::POSITION_STATE:
            command = "p:29xx0A02000310100000";
            break;
        case ID::ACTUAL_PRESSURE:
            command = "p:29xx0A02000407010000";
            break;
        case ID::TARGET_PRESSURE_USED:
            command = "p:29xx0A02000507030000";
            break;
        case ID::WARNING_BITMAP:
            command = "p:29xx0A0200060F300100";
            break;
        default:
            return "[ERROR] -1 Case not defined";;
    }

    if (id == ID::ACCESS_MODE || id == ID::CONTROL_MODE || id == ID::ACTUAL_POSITION)
    {
        if (id == ID::CONTROL_MODE)
        {
            String compoundCommand = "p:29xx0A0200000F020000";
            compoundCommand += String(";") + String(index);
            command = compoundCommand;
        }
    }

    client.println(command);

    unsigned long timeout = millis() + 1000;
    while (millis() < timeout)
    {
        if (client.available())
        {
            response = client.readStringUntil('\n');
            break;
        }
    }

    if (response.length() == 0)
    {
        Serial.println("No response or timed out");
        return "";
    }

    Serial.println("Response: " + response);
    return response;
}

