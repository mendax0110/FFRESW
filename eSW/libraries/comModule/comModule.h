#ifndef COMMODULE_H
#define COMMODULE_H

#include <Arduino.h>
#include <Ethernet.h>
#include "ETHH.h"
#include "I2CC.h"
#include "SER.h"
#include "SPII.h"

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
         * @brief Get the Ethernet object
         *
         * @return EthernetCommunication&
         */
        EthernetCommunication& getEthernet();

        /**
         * @brief Get the I2C object
         *
         * @return I2CCommunication&
         */
        I2CCommunication& getI2C();

        /**
         * @brief Get the SPI object
         *
         * @return SPICommunication&
         */
        SPICommunication& getSPI();

        /**
         * @brief Get the Serial object
         *
         * @return SerialCommunication&
         */
        SerialCommunication& getSerial();

    private:
        EthernetCommunication eth;
        I2CCommunication i2c;
        SPICommunication spi;
        SerialCommunication ser;
    };
}

#endif // COMMODULE_H
