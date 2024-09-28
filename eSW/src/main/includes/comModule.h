/**
 * @file comModule.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMODULE_H
#define COMMODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

/// @brief Namespace for the communication module. \namespace comModule
namespace comModule
{
    /// @brief Class for the communication module internals. \class comModuleInternals
    class comModuleInternals
    {
    public:
        comModuleInternals();
        ~comModuleInternals();
        
        void beginI2C(uint8_t address);
        void i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length);
        void i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length);

        void beginSPI();
        void spiWrite(uint8_t* data, size_t length);
        void spiRead(uint8_t* buffer, size_t length);
        
        void beginEthernet(byte* macAddress, IPAddress ip);
        void sendEthernetData(const char* data);
        void receiveEthernetData(char* buffer, size_t length);
        void handleEthernetClient();

    private:
        EthernetServer* server;
        EthernetClient client;
        bool ethernetInitialized;
    };
}

#endif // COMMODULE_H
