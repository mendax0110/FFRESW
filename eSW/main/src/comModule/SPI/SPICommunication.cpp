/**
 * @file SPICommunication.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "SPICommunication.h"

/// @brief Initializes the SPI communication
void SPICommunication::beginSPI()
{
    SPI.begin();
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
