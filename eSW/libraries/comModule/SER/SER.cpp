/**
 * @file SerialCommunication.cpp
 * @brief Implementation of the Serial communication class.
 */

#include "SER.h"

using namespace comModule;

void SerialCommunication::beginSerial(long baudRate)
{
    Serial.begin(baudRate);
    serInitialized = true;
}

void SerialCommunication::endSerial()
{
	Serial.end();
	serInitialized = false;
}

void SerialCommunication::sendSerialData(const char* data)
{
    Serial.println(data);
}

void SerialCommunication::receiveSerialData(char* buffer, size_t length)
{
    size_t index = 0;
    while (Serial.available() && index < length)
    {
        buffer[index++] = Serial.read();
    }
    buffer[index] = '\0'; // Null-terminate the string
}

bool SerialCommunication::isInitialized()
{
	return serInitialized;
}
