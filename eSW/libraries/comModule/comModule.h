#ifndef COMMODULE_H
#define COMMODULE_H

#include <Arduino.h>
#include <Ethernet.h>
#include "ETHH/ETHH.h"
#include "I2CC/I2CC.h"
#include "SER/SER.h"
#include "SPII/SPII.h"

/// @brief Namespace for the communication module. \namespace comModule
namespace comModule
{
    /// @brief Class for the communication module internals. \class comModuleInternals
    class ComModuleInternals
    {
    public:
        ComModuleInternals();
        ~ComModuleInternals();

        /**
         * @brief I2C communication object.
         *
         */
        I2CCommunication i2c;

        /**
         * @brief SPI communication object.
         *
         */
        SPICommunication spi;

        /**
         * @brief Ethernet communication object.
         *
         */
        EthernetCommunication eth;
        //SerialCommunication serial;
    };
}

#endif // COMMODULE_H
