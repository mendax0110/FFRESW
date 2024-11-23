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

namespace comModule
{
	/// @brief Class to handle I2C communication \class I2CCommunication
	class I2CCommunication
	{
	public:

		void beginI2C(uint8_t address);
		void i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length);
		size_t i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length);

		//constexpr uint8_t DEVICEADDRESS = 0x78;
	};
}

#endif // I2C_COMMUNICATION_H
