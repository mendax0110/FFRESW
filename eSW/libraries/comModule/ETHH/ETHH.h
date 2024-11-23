/**
 * @file EthernetCommunication.h
 * @brief Handles Ethernet communication for the comModule
 * @version 0.1
 * @date 2024-09-28
 */

#ifndef ETHERNET_COMMUNICATION_H
#define ETHERNET_COMMUNICATION_H

#include <Arduino.h>
#include <Ethernet.h>

namespace comModule
{
	/// @brief Class to handle Ethernet communication \class EthernetCommunication
	class EthernetCommunication
	{
	public:
		EthernetCommunication(); // Constructor declaration
		~EthernetCommunication();

		void beginEthernet(uint8_t* macAddress, IPAddress ip);
		void sendEthernetData(const char* data);
		void receiveEthernetData(char* buffer, size_t length);
		void handleEthernetClient();

		bool isInitialized();

	private:
		EthernetServer server; // Change to instance and will be initialized in constructor
		EthernetClient client; // Change to instance
		bool ethernetInitialized;
	};
}

#endif // ETHERNET_COMMUNICATION_H


