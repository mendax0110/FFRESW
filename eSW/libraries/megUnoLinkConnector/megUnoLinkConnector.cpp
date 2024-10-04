/**
 * @file megUnoLinkConnector.cpp
 * @brief Implementation of the MegUnoLinkConnector class using CommandHandler<> for handling MegunoLink serial communication.
 * @version 0.2
 * @date 2024-01-26
 */

#include "megUnoLinkConnector.h"
#include <Arduino.h>

using namespace megUnoLinkConnector;

MegUnoLinkConnector::MegUnoLinkConnector()
    : initialized(false)
{
}

MegUnoLinkConnector::~MegUnoLinkConnector() {}

/// @brief Function to initialize the MegUnoLink Connector 
/// @param baudRate -> This is the baud rate for the serial communication
void MegUnoLinkConnector::beginMegUno(long baudRate)
{
    if (!initialized)
    {
        Serial.begin(baudRate);
        Serial.println(F("MegUnoLink Connector Initialized."));
        initialized = true;
    }
    else
    {
        Serial.println(F("MegUnoLink Connector already initialized!"));
    }
}

/// @brief Function to process the commands received over serial communication
void MegUnoLinkConnector::processCommands()
{
    if (checkInitialized())
    {
        serialCommandHandler.Process();
    }
    else
    {
        sendLog(F("MegUnoLink Connector not initialized, skipping command processing."));
    }
}

/// @brief Function to add a command with a handler to the MegUnoLink Connector
/// @param handler -> This is the handler function for the command
void MegUnoLinkConnector::setDefaultHandler(void (*handler)())
{
    if (checkInitialized())
    {
        serialCommandHandler.SetDefaultHandler(handler);
        Serial.println(F("Default handler set."));
    }
    else
    {
        sendLog(F("MegUnoLink Connector not initialized, cannot set default handler."));
    }
}

/// @brief Function to handle unknown commands received over serial communication
void MegUnoLinkConnector::handleUnknownCommand()
{
    Serial.println(F("Unknown command received. Please try again."));
}

/// @brief Function to send a log message over serial communication
/// @param message -> This is the message to be logged
void MegUnoLinkConnector::sendLog(const String &message)
{
    if (checkInitialized())
    {
        Serial.println(message);
    }
    else
    {
        Serial.println(F("Failed to log: MegUnoLink Connector not initialized."));
    }
}

/// @brief Function to check if the MegUnoLink Connector is initialized
/// @return bool -> This returns true if the connector is initialized, false otherwise
bool MegUnoLinkConnector::checkInitialized()
{
    if (!initialized)
    {
        Serial.println(F("MegUnoLink Connector not initialized."));
    }
    return initialized;
}
