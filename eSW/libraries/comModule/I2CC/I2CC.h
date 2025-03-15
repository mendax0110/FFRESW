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

		/**
		 * @brief Function to initialize the I2C communication
		 *
		 * @param address -> The address of the I2C device
		 */
		void beginI2C(uint8_t address);

		/**
		 * @brief Function to end the I2C communication
		 *
		 */
		void endI2C();

		/**
		 * @brief Function to write data to the I2C device
		 *
		 * @param deviceAddress -> The address of the I2C device
		 * @param data -> The data to write
		 * @param length -> The length of the data
		 */
		void i2cWrite(uint8_t deviceAddress, uint8_t* data, size_t length);

		/**
		 * @brief Function to read data from the I2C device
		 *
		 * @param deviceAddress -> The address of the I2C device
		 * @param buffer -> The buffer to read the data into
		 * @param length -> The length of the data to read
		 * @return size_t -> The number of bytes read
		 */
		size_t i2cRead(uint8_t deviceAddress, uint8_t* buffer, size_t length);

		/**
		 * @brief Function to check if the I2C communication is initialized
		 *
		 * @return true -> if the I2C communication is initialized
		 * @return false -> if the I2C communication is not initialized
		 */
		bool isInitialized();

	private:
		bool i2cInitialized;
	};
}

#endif // I2C_COMMUNICATION_H
