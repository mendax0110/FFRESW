/**
 * @file comModule.cpp
 * @brief Implementation of the comModule class that utilizes various communication protocols.
 */

#include <Arduino.h>
#include <Ethernet.h>
#include "comModule.h"
#include "ETHH/ETHH.h"
#include "I2CC/I2CC.h"
#include "SER/SER.h"
#include "SPII/SPII.h"

using namespace comModule;

ComModuleInternals::ComModuleInternals()
{
    // Constructor does nothing until begin functions are called
}

ComModuleInternals::~ComModuleInternals()
{
    // Destructor (if needed for cleanup)
}

EthernetCommunication& ComModuleInternals::getEthernet()
{
    return eth;
}

I2CCommunication& ComModuleInternals::getI2C()
{
    return i2c;
}

SPICommunication& ComModuleInternals::getSPI()
{
    return spi;
}

SerialCommunication& ComModuleInternals::getSerial()
{
     return ser;
}

