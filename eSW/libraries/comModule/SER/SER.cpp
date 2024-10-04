/**
 * @file SerialCommunication.cpp
 * @brief Implementation of the Serial communication class.
 */

#include "SER.h"

using namespace comModule;

/// @brief Function to initialize the serial communication
/// @param baudRate -> The baud rate to use for the serial communication
void SerialCommunication::beginSerial(long baudRate)
{
    Serial.begin(baudRate);
}

/// @brief Function to send data over serial
/// @param data -> The data to send
void SerialCommunication::sendSerialData(const char* data)
{
    Serial.println(data);
}

/// @brief Function to receive data over serial
/// @param buffer -> The buffer to read the data into
/// @param length -> The length of the data to read
void SerialCommunication::receiveSerialData(char* buffer, size_t length)
{
    size_t index = 0;
    while (Serial.available() && index < length)
    {
        buffer[index++] = Serial.read();
    }
    buffer[index] = '\0'; // Null-terminate the string
}
