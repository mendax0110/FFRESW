/**
 * @file I2CCommunication.h
 * @brief Handles I2C communication for the comModule
 * @version 0.1
 * @date 2024-09-28
 */

#ifndef I2C_COMMUNICATION_H
#define I2C_COMMUNICATION_H

#include <Arduino.h>
#include <Wire.h>

/// @brief Class to handle I2C communication \class I2CCommunication
class I2CCommunication
{
public:

    void beginI2C(uint8_t address);
    void i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length);
    void i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length);
};

#endif // I2C_COMMUNICATION_H
