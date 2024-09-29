/**
 * @file SPICommunication.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SPI_COMMUNICATION_H
#define SPI_COMMUNICATION_H

#include <Arduino.h>
#include <SPI.h>

/// @brief Class to handle SPI communication \class SPICommunication
class SPICommunication
{
public:

    void beginSPI();
    void spiWrite(uint8_t* data, size_t length);
    void spiRead(uint8_t* buffer, size_t length);
};

#endif // SPI_COMMUNICATION_H
