/**
 * @file EthernetCommunication.cpp
 * @brief Implementation of the Ethernet communication class.
 */
#include "ETHH.h"
#include <Ethernet.h>
#include <Arduino.h>

using namespace comModule;

EthernetCommunication::EthernetCommunication() : ethernetInitialized(false), server(80)
{

}

EthernetCommunication::~EthernetCommunication()
{

}

/// @brief Function to initialize the Ethernet communication
/// @param macAddress -> The MAC address to use for the Ethernet communication
/// @param ip -> The IP address to use for the Ethernet communication
void EthernetCommunication::beginEthernet(uint8_t* macAddress, IPAddress ip)
{
    Ethernet.begin(macAddress, ip);
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware.");
        ethernetInitialized = false;
        return;
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
      ethernetInitialized = false;
      return;
    }
    server.begin();
    ethernetInitialized = true;
    Serial.print("Client is at: ");
    Serial.println(Ethernet.localIP());
}

/// @brief Function to check if the Ethernet communication is initialized
/// @return -> true if the Ethernet communication is initialized, false otherwise
bool EthernetCommunication::isInitialized()
{
	return ethernetInitialized;
}

/// @brief Get the currently active Ethernet client
/// @return Reference to the active Ethernet client
EthernetClient& EthernetCommunication::getClient()
{
    return client;
}

/// @brief Function to receive data over Ethernet
/// @param buffer -> The buffer to read the data into
/// @param length -> The length of the data to read
void EthernetCommunication::receiveEthernetData(char* buffer, size_t length)
{
    if (!ethernetInitialized) return;

    if (client.available())
    {
        size_t bytesRead = client.readBytes(buffer, length);
        buffer[bytesRead] = '\0';
    }
}

/// @brief Function to get the requested endpoint
String EthernetCommunication::getRequestedEndpoint()
{
    EthernetClient newClient = server.available();
    String requestedEndpoint = "";

    if (newClient)
    {
        client = newClient;

        if (client.connected())
        {
            char request[256] = {0};
            int bytesRead = client.readBytesUntil('\n', request, sizeof(request) - 1);

            if (bytesRead > 0)
            {
                String requestStr = String(request);
                //Serial.println("Received request:");
                //Serial.println(requestStr);

                int startIdx = requestStr.indexOf("GET /") + 5;
                int endIdx = requestStr.indexOf(" ", startIdx);

                if (startIdx > 0 && endIdx > startIdx)
                {
                    requestedEndpoint = requestStr.substring(startIdx, endIdx);
                    requestedEndpoint.trim();
                }

                client.println(F("HTTP/1.1 200 OK"));
                client.println(F("Content-Type: application/json"));
                client.println(F("Connection: close"));
                client.println();
            }
        }
    }

    return requestedEndpoint;
}

/// @brief Function to send the json response with the measurment data
/// @param jsonBody -> jsonstring with the content needed
void EthernetCommunication::sendJsonResponse(const String& jsonBody)
{
    EthernetClient activeClient = getClient();

    if (activeClient && activeClient.connected())
    {
        activeClient.println(jsonBody.c_str());
        activeClient.stop();
    }
    else
    {
        Serial.println("Error: No active client to send JSON response.");
    }
}

/// @brief Function to handle the Ethernet client
void EthernetCommunication::handleEthernetClient()
{
    EthernetClient newClient = server.available();

    if (newClient)
    {
        client = newClient;

        if (client.connected())
        {
            char request[256] = {0};
            int bytesRead = client.readBytesUntil('\n', request, sizeof(request) - 1);

            if (bytesRead > 0)
            {
                client.println(F("HTTP/1.1 200 OK"));
                client.println(F("Content-Type: application/json"));
                client.println(F("Connection: close"));
                client.println();
            }
        }
        client.stop();
    }
}

/// @brief Fucntion to send data over Ethernet
/// @param endpoint -> endpoint to send data to
/// @param data -> The data to send
void EthernetCommunication::sendEthernetData(const char* endpoint, const char* data)
{
    if (!ethernetInitialized) return;

    EthernetClient activeClient = server.available();
    if (activeClient.connected())
    {
        // Send HTTP header
        activeClient.println("POST " + String(endpoint) + " HTTP/1.1");
        activeClient.println("Host: ESW");
        activeClient.println("Content-Type: application/json");
        activeClient.println("Connection: close");

        // Send JSON data
        activeClient.println("Content-Length: " + String(strlen(data)));
        activeClient.println();
        activeClient.println(data);

        activeClient.stop();
    }
}

/// @brief Function to get the current status of the flag
/// @return sendDataFlag -> true if data should be sent, false otherwise
bool EthernetCommunication::getSendDataFlag() const
{
	return sendDataFlag;
}

/// @brief Function to get the current status of the flag
/// @param flag -> set the flag to true if data sent, false otherwise
void EthernetCommunication::setSendDataFlag(bool flag)
{
	sendDataFlag = flag;
}


/// @brief Transforms the given float to IEEE-754 float values.
/// @param value -> float we want to convert
/// @return String of the converted float
String EthernetCommunication::floatToIEEE754(float value)
{
	union
	{
		float f;
		uint32_t i;
	} converter;
	converter.f = value;

	char buffer[9];
	snprintf(buffer, sizeof(buffer), "%08X", converter.i);
	return String(buffer);
}

/// @brief Parses the response and extracts IEEE-754 float values.
/// @param response -> Raw response string containing IEEE-754 hex values.
/// @return Vector<float> containing parsed float values.
Vector<float> EthernetCommunication::parseResponse(String response)
{
	Vector<float> parsedValue;
	int start = 0;

	while (start <= response.length() - 8)
	{
		String hexValue = response.substring(start, start + 8);
		bool isValidHex = true;

		for (char c : hexValue)
		{
			if (!isxdigit(c))
			{
				isValidHex = false;
				break;
			}
		}

		if (isValidHex)
		{
			// Convert the hex string to a float (IEEE-754 conversion)
			unsigned long hexInt = strtoul(hexValue.c_str(), NULL, 16);
			float floatValue = *reinterpret_cast<float*>(&hexInt);
			parsedValue.push_back(floatValue);
		}

		start += 8;
	}

	return parsedValue;
}

/// @brief Function to set a compound command for the valve uC Slave (Compound1)
/// @param id Enum ID from Compound1
/// @param index Index of the command
/// @param value Value of the command
void EthernetCommunication::setCompound(Compound1 id, int index, String value)
{
    setCompoundInternal("0A0100", static_cast<unsigned long>(id), index, value);
}

/// @brief Function to set a compound command for the valve uC Slave (Compound2)
/// @param id Enum ID from Compound2
/// @param index Index of the command
/// @param value Value of the command
void EthernetCommunication::setCompound(Compound2 id, int index, String value)
{
    setCompoundInternal("0A0200", static_cast<unsigned long>(id), index, value);
}

/// @brief Function to set a compound command for the valve uC Slave (Compound3)
/// @param id Enum ID from Compound3
/// @param index Index of the command
/// @param value Value of the command
void EthernetCommunication::setCompound(Compound3 id, int index, String value)
{
    setCompoundInternal("0A0300", static_cast<unsigned long>(id), index, value);
}

/// @brief Internal function to send the command over Ethernet
void EthernetCommunication::setCompoundInternal(String compoundType, unsigned long id, int index, String value)
{
    if (!ethernetInitialized) return;

    String command = "p:28" + compoundType + "00";
    command += String(id, HEX);

    if (value.indexOf('.') != -1) // Corrected condition
    {
        value = floatToIEEE754(value.toFloat());
    }

    command += ";" + String(index) + ";" + value;

    if (client.connect("192.168.1.10", 503))
    {
        client.println(command);
        client.stop();
    }
}

/// @brief Function to get a compound command response from the valve uC Slave (Compound1)
/// @param id Enum ID from Compound1
/// @param index Index of the command
/// @return String Response from the valve uC slave
String EthernetCommunication::getCompound(Compound1 id, int index)
{
    return getCompoundInternal("0A0100", static_cast<unsigned long>(id), index);
}

/// @brief Function to get a compound command response from the valve uC Slave (Compound2)
/// @param id Enum ID from Compound2
/// @param index Index of the command
/// @return String Response from the valve uC slave
String EthernetCommunication::getCompound(Compound2 id, int index)
{
    return getCompoundInternal("0A0200", static_cast<unsigned long>(id), index);
}

/// @brief Function to get a compound command response from the valve uC Slave (Compound3)
/// @param id Enum ID from Compound3
/// @param index Index of the command
/// @return String Response from the valve uC slave
String EthernetCommunication::getCompound(Compound3 id, int index)
{
    return getCompoundInternal("0A0300", static_cast<unsigned long>(id), index);
}

/// @brief Internal function to handle getting a compound response over Ethernet
String EthernetCommunication::getCompoundInternal(String compoundType, unsigned long id, int index)
{
    if (!ethernetInitialized) return "[ERROR] ethernetshield not initialized";

    String command = "p:29" + compoundType + "00";
    command += String(id, HEX);
    command += ";" + String(index);

    if (!client.connect("192.168.1.10", 503))
    {
    	Serial.println("[ERROR] Failed to connect to device");
    	return "";
    }

    client.println(command);

    String response;
    unsigned long timeout = millis() + 1000;
    while (millis() < timeout)
    {
        if (client.available())
        {
            response = client.readStringUntil('\n');
            break;
        }
    }

    client.stop();

    if (response.length() == 0)
    {
        Serial.println("No response or timed out");
        return "";
    }

    Serial.println("Response: " + response);
    return response;
}

/// @brief Function to parse a compound response into a vector (Compound1)
Vector<float> EthernetCommunication::getParsedCompound(Compound1 id, int index)
{
    return parseCompoundResponse(getCompound(id, index));
}

/// @brief Function to parse a compound response into a vector (Compound2)
Vector<float> EthernetCommunication::getParsedCompound(Compound2 id, int index)
{
    return parseCompoundResponse(getCompound(id, index));
}

/// @brief Function to parse a compound response into a vector (Compound3)
Vector<float> EthernetCommunication::getParsedCompound(Compound3 id, int index)
{
    return parseCompoundResponse(getCompound(id, index));
}

/// @brief Internal function to parse a response string into a float vector
Vector<float> EthernetCommunication::parseCompoundResponse(String response)
{
    if (response.length() == 0)
    {
        Serial.println("[ERROR] No valid response received.");
        return Vector<float>();
    }

    Vector<float> parsedResponse = parseResponse(response);

    Serial.print("Parsed Values: ");
    for (size_t i = 0; i < parsedResponse.size(); i++)
    {
        Serial.print(parsedResponse[i], 6);
        Serial.print(" ");
    }
    Serial.println();

    return parsedResponse;
}
