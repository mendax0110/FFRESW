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

namespace comModule
{
	/// @brief Class to handle SPI communication \class SPICommunication
	class SPICommunication
	{
	public:

		void beginSPI();
		void endSPI();
		void spiWrite(uint8_t* data, size_t length);
		void spiRead(uint8_t* buffer, size_t length);
		bool isInitialized();

	private:
		bool spiInitialized;
	};
}

#endif // SPI_COMMUNICATION_H
