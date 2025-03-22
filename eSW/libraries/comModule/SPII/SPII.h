/**
 * @file SPICommunication.h
 * @brief Handles SPI communication for the comModule
 * @version 0.1
 * @date 2024-09-28
 */

#ifndef SPI_COMMUNICATION_H
#define SPI_COMMUNICATION_H

#include "SPII.h"

#include <Arduino.h>

/// @brief Namespace for the communication module \namespace comModule
namespace comModule
{
	/// @brief Class to handle SPI communication \class SPICommunication
	class SPICommunication
	{
	public:
		SPICommunication();
		~SPICommunication();

		/**
		 * @brief Function to initialize the SPI communication
		 *
		 */
		void beginSPI();

		/**
		 * @brief Function to end the SPI communication
		 *
		 */
		void endSPI();

		/**
		 * @brief Function to write data over SPI
		 *
		 * @param data -> The data to write
		 * @param length -> The length of the data
		 */
		void spiWrite(uint8_t* data, size_t length);

		/**
		 * @brief Function to read data over SPI
		 *
		 * @param buffer -> The buffer to read the data into
		 * @param length -> The length of the data to read
		 */
		void spiRead(uint8_t* buffer, size_t length);

		/**
		 * @brief Function to check if the SPI communication is initialized
		 *
		 * @return true -> if the SPI communication is initialized
		 * @return false -> if the SPI communication is not initialized
		 */
		bool isInitialized() const;

	private:
		bool spiInitialized = false;
	};
}

#endif // SPI_COMMUNICATION_H
