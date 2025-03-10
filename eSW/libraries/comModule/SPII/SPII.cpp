/**
 * @file SPICommunication.cpp
 * @brief Implementation of the SPI communication class.
 */

#include "SPII.h"
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>

using namespace comModule;

/// @brief Function to check if the SPI communication is initialized
/// @return -> true if the SPI communication is initialized, false otherwise
bool SPICommunication::isInitialized()
{
	return spiInitialized;
}

/// @brief Initializes the SPI communication
void SPICommunication::beginSPI()
{
    SPI.begin();
    spiInitialized = true;
}

/// @brief Ends the SPI communication
void SPICommunication::endSPI()
{
	SPI.end();
	spiInitialized = false;
}

/// @brief Function to write data over SPI
/// @param data -> The data to write
/// @param length -> The length of the data
void SPICommunication::spiWrite(uint8_t* data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        SPI.transfer(data[i]);
    }
}

/// @brief Function to read data over SPI
/// @param buffer -> The buffer to read the data into
/// @param length -> The length of the data to read
void SPICommunication::spiRead(uint8_t* buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        buffer[i] = SPI.transfer(0x00);
    }
}
