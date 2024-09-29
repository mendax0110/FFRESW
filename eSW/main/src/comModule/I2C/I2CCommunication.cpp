/**
 * @file I2CCommunication.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "I2CCommunication.h"

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
void I2CCommunication::i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length)
{
    Wire.requestFrom(deviceAddress, length);
    size_t index = 0;
    while (Wire.available() && index < length)
    {
        buffer[index++] = Wire.read();
    }
}
