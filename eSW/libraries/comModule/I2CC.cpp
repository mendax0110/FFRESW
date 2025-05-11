/**
 * @file I2CCommunication.cpp
 * @brief Implementation of the I2C communication class.
 */

#include "I2CC.h"
#include <Wire.h>

using namespace comModule;

I2CCommunication::I2CCommunication()
{

}

I2CCommunication::~I2CCommunication()
{
	endI2C();
}

bool I2CCommunication::isInitialized() const
{
	return i2cInitialized;
}

void I2CCommunication::beginI2C(uint8_t address)
{
    Wire.begin(address);
    i2cInitialized = true;
}

void I2CCommunication::endI2C()
{
	Wire.end();
	i2cInitialized = false;
}

void I2CCommunication::i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(data, length);
    Wire.endTransmission();
}

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

