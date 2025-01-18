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
	enum class service
	{
		SET = 28,
		GET = 29,
		SETGET = 30
	};

	enum class ID : unsigned long
	{
		ACCESS_MODE = 0x0F0B0000,
		CONTROL_MODE = 0x0F020000,
		ACTUAL_POSITION = 0x10010000,
		TARGET_POSITION = 0x11020000,
		TARGET_PRESSURE = 0x07020000,
		POSITION_STATE = 0x00100000,
		ACTUAL_PRESSURE = 0x07010000,
		TARGET_PRESSURE_USED = 0x07030000,
		WARNING_BITMAP = 0x0F300100
	};

	/// @brief Class to handle Ethernet communication \class EthernetCommunication
	class EthernetCommunication
	{
	public:
		EthernetCommunication();
		~EthernetCommunication();

		void beginEthernet(uint8_t* macAddress, IPAddress ip);
		void sendEthernetData(const char* endpoint, const char* data);
		void receiveEthernetData(char* buffer, size_t length);
		void handleEthernetClient();

		bool isInitialized();

		bool getSendDataFlag() const;
		void setSendDataFlag(bool flag);


		void setCompound(ID id, int index, String value);
		String getCompound(ID id, int index);


	private:
		EthernetServer server;
		EthernetClient client;
		bool ethernetInitialized;
		bool sendDataFlag = false;
	};
}

#endif // ETHERNET_COMMUNICATION_H


