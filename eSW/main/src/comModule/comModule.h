/**
 * @file comModule.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMODULE_H
#define COMMODULE_H

#include "I2C/I2CCommunication.h"
#include "SPI/SPICommunication.h"
#include "ETH/EthernetCommunication.h"
#include "SER/SerialCommunication.h"

/// @brief Namespace for the communication module. \namespace comModule
namespace comModule
{
    /// @brief Class for the communication module internals. \class comModuleInternals
    class comModuleInternals
    {
    public:
        comModuleInternals();
        ~comModuleInternals();

        I2CCommunication i2c;
        SPICommunication spi;
        EthernetCommunication ethernet;
        SerialCommunication serial;
    };
}

#endif // COMMODULE_H
