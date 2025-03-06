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
#include <Vector.h>

namespace comModule
{
	enum class Service : uint8_t
	{
		SET = 0x01,
		GET = 0x0B,
		SET_COMPOUND = 0x28,
		GET_COMPOUND = 0x29,
		SETGET = 0x30
	};

	enum class Compound1 : uint32_t
	{
		CONTROL_MODE = 0x0F020000,
		TARGET_POSITION = 0x11020000,
		TARGET_PRESSURE = 0x07020000,
		NOT_USED = 0x00000000
	};

	enum class Compound2 : uint32_t
	{
	    ACCESS_MODE = 0x0F0B0000,
	    CONTROL_MODE = 0x0F020000,
		TARGET_POSITION = 0x11020000,
		TARGET_PRESSURE = 0x07020000,
	    ACTUAL_POSITION = 0x10010000,
	    POSITION_STATE = 0x00100000,
	    ACTUAL_PRESSURE = 0x07010000,
	    TARGET_PRESSURE_USED = 0x07030000,
	    WARNING_BITMAP = 0x0F300100,
	    NOT_USED = 0x00000000
	};

	enum class Compound3 : uint32_t
	{
		CONTROL_MODE = 0x0F020000,
		TARGET_POSITION = 0x11020000,
		TARGET_PRESSURE = 0x07020000,
		SEPARATION = 0x00000000,
		ACCESS_MODE = 0x0F0B0000,
		ACTUAL_POSITION = 0x10010000,
		POSITION_STATE = 0x00100000,
		ACTUAL_PRESSURE = 0x07010000,
		TARGET_PRESSURE_USED = 0x07030000,
		WARNING_BITMAP = 0x0F300100,
		NOT_USED = 0x00000000
	};

	enum class Error_Codes : uint8_t
	{
		NO_ERROR = 0x00,
		WRONG_COMMAND_LENGTH = 0x0C,
		VALUE_TOO_LOW = 0x1C,
		VALUE_TOO_HIGH = 0x1D,
		RESULTING_ZERO_ADJUST_OFFSET = 0x20,
		NO_SENSOR_ENABLED = 0x21,
		WRONG_ACCESS_MODE = 0x50,
		TIMEOUT = 0x51,
		NV_MEMORY_NOT_READY = 0x6D,
		WRONG_PARAMETER_ID = 0x6E,
		PARAMETER_NOT_SETTABLE = 0x70,
		PARAMETER_NOT_READABLE = 0x71,
		WRONG_PARAMETER_INDEX = 0x73,
		WRONG_VALUE_WITHIN_RANGE = 0x76,
		NOT_ALLOWED_IN_THIS_STATE = 0x78,
		SETTING_LOCK = 0x79,
		WRONG_SERVICE = 0x7A,
		PARAMETER_NOT_ACTIVE = 0x7B,
		PARAMETER_SYSTEM_ERROR = 0x7C,
		COMMUNICATION_ERROR = 0x7D,
		UNKNOWN_SERVICE = 0x7E,
		UNEXPECTED_CHARACTER = 0x7F,
		NO_ACCESS_RIGHTS = 0x80,
		NO_ADEQUATE_HARDWARE = 0x81,
		WRONG_OBJECT_STATE = 0x82,
		NO_SLAVE_COMMAND = 0x84,
		COMMAND_TO_UNKNOWN_SLAVE = 0x85,
		COMMAND_TO_MASTER_ONLY = 0x87,
		ONLY_G_COMMAND_ALLOWED = 0x88,
		NOT_SUPPORTED = 0x89,
		FUNCTION_DISABLED = 0xA0,
		ALREADY_DONE = 0xA1
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
		String getRequestedEndpoint();
		void sendJsonResponse(const String& jsonBody);

		EthernetClient& getClient();

		bool isInitialized();

		bool getSendDataFlag() const;
		void setSendDataFlag(bool flag);

		void setCompound(Compound1 id, int index, String value);
		void setCompound(Compound2 id, int index, String value);
		void setCompound(Compound3 id, int index, String value);
		void setCompoundInternal(String compoundType, unsigned long id, int index, String value);

		String getCompound(Compound1 id, int index);
		String getCompound(Compound2 id, int index);
		String getCompound(Compound3 id, int index);
		String getCompoundInternal(String compoundType, unsigned long id, int index);

		Vector<float> getParsedCompound(Compound1 id, int index);
		Vector<float> getParsedCompound(Compound2 id, int index);
		Vector<float> getParsedCompound(Compound3 id, int index);
		Vector<float> parseCompoundResponse(String response);

		void setParameter(Compound2 id, String value);
		String getParameter(Compound2 id);
		void sendCommand(String command);

	private:
		EthernetServer server;
		EthernetClient client;
		bool ethernetInitialized;
		bool sendDataFlag = false;

		String floatToIEEE754(float value);
		Vector<float> parseResponse(String response);

	};
}

#endif // ETHERNET_COMMUNICATION_H


