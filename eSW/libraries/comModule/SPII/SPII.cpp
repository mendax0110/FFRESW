/**
 * @file SPICommunication.cpp
 * @brief Implementation of the SPI communication class.
 */

#include "SPII.h"
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>

using namespace comModule;

bool SPICommunication::isInitialized()
{
	return spiInitialized;
}

void SPICommunication::beginSPI()
{
    SPI.begin();
    spiInitialized = true;
}

void SPICommunication::endSPI()
{
	SPI.end();
	spiInitialized = false;
}

void SPICommunication::spiWrite(uint8_t* data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        SPI.transfer(data[i]);
    }
}

void SPICommunication::spiRead(uint8_t* buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        buffer[i] = SPI.transfer(0x00);
    }
}
