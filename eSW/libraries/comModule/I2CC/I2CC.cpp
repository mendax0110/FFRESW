/**
 * @file I2CCommunication.cpp
 * @brief Implementation of the I2C communication class.
 */

#include "I2CC.h"

#include <Wire.h>

using namespace comModule;

/// @brief Function to initialize the I2C communication
/// @param address -> The address to use for the I2C communication
void I2CCommunication::beginI2C(uint8_t address)
{
    Wire.begin(address);
}

/// @brief Function to write data over I2C
/// @param deviceAddress -> The address of the device to write to
/// @param data -> The data to write
/// @param length -> The length of the data
void I2CCommunication::i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(data, length);
    Wire.endTransmission();
}

/// @brief Function to read data over I2C
/// @param deviceAddress -> The address of the device to read from
/// @param buffer -> The buffer to read the data into
/// @param length -> The length of the data to read
size_t I2CCommunication::i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length)
{
    size_t bytesRead = 0;
    Wire.requestFrom(deviceAddress, length);
    while (Wire.available() && bytesRead < length)
    {
        buffer[bytesRead++] = Wire.read();
    }
    return bytesRead;
}

