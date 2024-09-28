/**
 * @file comModule.cpp
 * @author Adrian Goessl
 * @brief Implementation of the comModule class.
 * @version 0.3
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "includes/comModule.h"

using namespace comModule;

comModuleInternals::comModuleInternals() : ethernetInitialized(false), server(nullptr) {}

comModuleInternals::~comModuleInternals()
{
    if (server)
    {
        delete server;
    }
}

/// @brief Function to initialize the I2C bus
/// @param address -> This is the address of the device
void comModuleInternals::beginI2C(uint8_t address)
{
    Wire.begin(address);
}

/// @brief Function to write data to an I2C device
/// @param deviceAddress -> This is the address of the device
/// @param data -> This is the data to be written
/// @param length -> This is the length of the data
void comModuleInternals::i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(data, length);
    Wire.endTransmission();
}

/// @brief Function to read data from an I2C device
/// @param deviceAddress -> This is the address of the device
/// @param buffer -> This is the buffer to store the data
/// @param length -> This is the length of the data
void comModuleInternals::i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length)
{
    Wire.requestFrom(deviceAddress, length);
    size_t index = 0;
    while (Wire.available() && index < length)
    {
        buffer[index++] = Wire.read();
    }
}

/// @brief Function to initialize the SPI bus
void comModuleInternals::beginSPI()
{
    SPI.begin();
}

/// @brief Function to write data over SPI
/// @param data -> This is the data to be written
/// @param length -> This is the length of the data
void comModuleInternals::spiWrite(uint8_t* data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        SPI.transfer(data[i]);
    }
}

/// @brief Function to read data over SPI
/// @param buffer -> This is the buffer to store the data
/// @param length -> This is the length of the data
void comModuleInternals::spiRead(uint8_t* buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        buffer[i] = SPI.transfer(0x00);
    }
}

/// @brief Function to initialize the Ethernet module
/// @param macAddress -> This is the MAC address of the device
/// @param ip -> This is the IP address of the device
void comModuleInternals::beginEthernet(byte* macAddress, IPAddress ip)
{
    Ethernet.begin(macAddress, ip);
    server = new EthernetServer(80); // TODO: MAYBE CHANGE PORT!!
    server->begin();
    ethernetInitialized = true;
}

/// @brief Function to send data over Ethernet
/// @param data -> This is the data to be sent
void comModuleInternals::sendEthernetData(const char* data)
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
/// @param buffer -> This is the buffer to store the data
/// @param length -> This is the length of the data
void comModuleInternals::receiveEthernetData(char* buffer, size_t length)
{
    if (!ethernetInitialized) return;

    client = server->available();
    if (client)
    {
        size_t bytesRead = client.readBytes(buffer, length);
        buffer[bytesRead] = '\0';
    }
}

/// @brief Function to handle Ethernet client requests
void comModuleInternals::handleEthernetClient()
{
    if (client)
    {
        // TODO: Add logic to handle requests
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
