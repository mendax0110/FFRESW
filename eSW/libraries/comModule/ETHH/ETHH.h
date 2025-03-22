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

/// @brief Namespace for the communication module \namespace comModule
namespace comModule
{
	/// @brief Enum class for the service types \enum Service
	enum class Service : uint8_t
	{
		SET = 0x01,
		GET = 0x0B,
		SET_COMPOUND = 0x28,
		GET_COMPOUND = 0x29,
		SETGET = 0x30
	};

	/// @brief Enum class for the compound 1 types \enum Compound1
	enum class Compound1 : uint32_t
	{
		CONTROL_MODE = 0x0F020000,
		TARGET_POSITION = 0x11020000,
		TARGET_PRESSURE = 0x07020000,
		NOT_USED = 0x00000000
	};

	/// @brief Enum class for the compound 2 types \enum Compound2
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

	/// @brief Enum class for the compound 3 types \enum Compound3
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

	/// @brief Enum class for the error codes \enum Error_Codes
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

		/**
		 * @brief Function to initialize the Ethernet communication
		 *
		 * @param macAddress -> The MAC address to use for the Ethernet communication
		 * @param ip -> The IP address to use for the Ethernet communication
		 */
		void beginEthernet(uint8_t* macAddress, IPAddress ip);

		/**
		 * @brief Function to send data over Ethernet
		 *
		 * @param endpoint -> endpoint to send data to
		 * @param data -> The data to send
		 */
		void sendEthernetData(const char* endpoint, const char* data);

		/**
		 * @brief Function to receive data over Ethernet
		 *
		 * @param buffer -> The buffer to read the data into
		 * @param length -> The length of the data to read
		 */
		void receiveEthernetData(char* buffer, size_t length);

		/**
		 * @brief Function to handle the Ethernet client
		 *
		 */
		void handleEthernetClient();

		/**
		 * @brief Function to get the requested endpoint
		 *
		 * @return String -> The requested endpoint
		 */
		String getRequestedEndpoint();

		/**
		 * @brief Function to get the specific endpoint
		 *
		 * @param jsonBody -> The json body to get the endpoint from
		 * @return String -> The specific endpoint
		 */
		String getSpecificEndpoint(const String& jsonBody);

		/**
		 * @brief Function to send the json response with the measurment data
		 *
		 * @param jsonBody -> jsonstring with the content needed
		 */
		void sendJsonResponse(const String& jsonBody);

		/**
		 * @brief Get the currently active Ethernet client
		 *
		 * @return EthernetClient& , Reference to the active Ethernet client
		 */
		EthernetClient& getClient();

		/**
		 * @brief Function to check if the Ethernet communication is initialized
		 *
		 * @return true -> if the Ethernet communication is initialized
		 * @return false -> if the Ethernet communication is not initialized
		 */
		bool isInitialized() const;

		/**
		 * @brief Function to get the current status of the flag
		 *
		 * @return true -> if data should be sent
		 * @return false -> if data should not be sent
		 */
		bool getSendDataFlag() const;

		/**
		 * @brief Function to get the current status of the flag
		 *
		 * @param flag -> set the flag to true if data sent, false otherwise
		 */
		void setSendDataFlag(bool flag);

		/**
		 * @brief Function to set a compound command for the valve uC Slave (Compound1)
		 *
		 * @param id -> Enum ID from Compound1
		 * @param index -> Index of the command
		 * @param value -> Value of the command
		 */
		void setCompound(Compound1 id, int index, String value);

		/**
		 * @brief Function to set a compound command for the valve uC Slave (Compound2)
		 *
		 * @param id -> Enum ID from Compound2
		 * @param index -> Index of the command
		 * @param value -> Value of the command
		 */
		void setCompound(Compound2 id, int index, String value);

		/**
		 * @brief Function to set a compound command for the valve uC Slave (Compound3)
		 *
		 * @param id -> Enum ID from Compound3
		 * @param index -> Index of the command
		 * @param value -> Value of the command
		 */
		void setCompound(Compound3 id, int index, String value);

		/**
		 * @brief Function to set the Interal compound command for the valve uC Slave
		 *
		 * @param compoundType -> The type of the compound
		 * @param id -> The ID of the compound
		 * @param index -> The index of the compound
		 * @param value -> The value of the compound
		 */
		void setCompoundInternal(String compoundType, unsigned long id, int index, String value);

		/**
		 * @brief Getter for a compound command response from the valve uC Slave (Compound1)
		 *
		 * @param id -> Enum ID from Compound1
		 * @param index -> Index of the command
		 * @return String -> Response from the valve uC slave
		 */
		String getCompound(Compound1 id, int index);

		/**
		 * @brief Getter for a compound command response from the valve uC Slave (Compound2)
		 *
		 * @param id -> Enum ID from Compound2
		 * @param index -> Index of the command
		 * @return String -> Response from the valve uC slave
		 */
		String getCompound(Compound2 id, int index);

		/**
		 * @brief Getter for a compound command response from the valve uC Slave (Compound3)
		 *
		 * @param id -> Enum ID from Compound3
		 * @param index -> Index of the command
		 * @return String -> Response from the valve uC slave
		 */
		String getCompound(Compound3 id, int index);

		/**
		 * @brief Getter for the internal compound command response from the valve uC Slave
		 *
		 * @param compoundType -> The type of the compound
		 * @param id -> The ID of the compound
		 * @param index -> The index of the compound
		 * @return String -> Response from the valve uC slave
		 */
		String getCompoundInternal(String compoundType, unsigned long id, int index);

		/**
		 * @brief Function to get a compound command response from the valve uC Slave (Compound1)
		 *
		 * @param id -> Enum ID from Compound1
		 * @param index -> Index of the command
		 * @return Vector<float> -> Response from the valve uC slave
		 */
		Vector<float> getParsedCompound(Compound1 id, int index);

		/**
		 * @brief Function to get a compound command response from the valve uC Slave (Compound2)
		 *
		 * @param id -> Enum ID from Compound1
		 * @param index -> Index of the command
		 * @return Vector<float> -> Response from the valve uC slave
		 */
		Vector<float> getParsedCompound(Compound2 id, int index);

		/**
		 * @brief Function to get a compound command response from the valve uC Slave (Compound3)
		 *
		 * @param id -> Enum ID from Compound1
		 * @param index -> Index of the command
		 * @return Vector<float> -> Response from the valve uC slave
		 */
		Vector<float> getParsedCompound(Compound3 id, int index);

		/**
		 * @brief Function to parse a compound response into a vector (Compound1)
		 *
		 * @param response -> Raw response string containing IEEE-754 hex values.
		 * @return Vector<float> -> containing parsed float values.
		 */
		Vector<float> parseCompoundResponse(String response);

		/**
		 * @brief Setter for a parameter from the VAT slave
		 *
		 * @param id -> The ID of the parameter to set
		 * @param value -> The value to set the parameter to
		 */
		void setParameter(Compound2 id, String value);

		/**
		 * @brief Getter for a parameter from the VAT slave.
		 *
		 * @param id -> The ID of the parameter to get
		 * @return -> String will return the value of the parameter as a string, otherwise an empty string or error message.
		 */
		String getParameter(Compound2 id);

		/**
		 * @brief Helper function to send a command to the VAT slave controller
		 *
		 * @param command -> The command to send to the VAT slave controller
		 */
		void sendCommand(String command);

	private:
		EthernetServer server;
		EthernetClient client;
		bool ethernetInitialized = false;
		bool sendDataFlag = false;

		/**
		 * @brief Transforms the given float to IEEE-754 float values.
		 *
		 * @param value -> float we want to convert
		 * @return String -> IEEE-754 float value as a string
		 */
		String floatToIEEE754(float value);

		/**
		 * @brief Parses the response and extracts IEEE-754 float values.
		 *
		 * @param response -> Raw response string containing IEEE-754 hex values.
		 * @return Vector<float> containing parsed float values.
		 */
		Vector<float> parseResponse(String response);

	};
}

#endif // ETHERNET_COMMUNICATION_H


