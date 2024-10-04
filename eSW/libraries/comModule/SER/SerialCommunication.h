/**
 * @file SerialCommunication.h
 * @brief Handles Serial communication for the comModule
 * @version 0.1
 * @date 2024-09-28
 */

#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <Arduino.h>

/// @brief Class to handle Serial communication \class SerialCommunication
class SerialCommunication
{
public:

    void beginSerial(long baudRate);
    void sendSerialData(const char* data);
    void receiveSerialData(char* buffer, size_t length);
};

#endif // SERIAL_COMMUNICATION_H
