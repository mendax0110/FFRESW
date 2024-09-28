/**
 * @file megUnoLinkConnector.cpp
 * @author Adrian Goessl
 * @brief Implementation of the megUnoLinkConnector class.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "includes/megUnoLinkConnector.h"

using namespace megUnoLinkConnector;

megUnoLinkConnectorInternals::megUnoLinkConnectorInternals() {}
megUnoLinkConnectorInternals::~megUnoLinkConnectorInternals() {}

void megUnoLinkConnectorInternals::begin(long baudRate)
{
    Serial.begin(baudRate);
    Serial.println(F("MegUnoLink Connector Initialized"));
}

void megUnoLinkConnectorInternals::processCommands()
{
    serialCommandHandler.Process();
}

void megUnoLinkConnectorInternals::addCommand(const __FlashStringHelper* command, void (*handler)(CommandParameter&))
{
    serialCommandHandler.AddCommand(command, handler);
}

void megUnoLinkConnectorInternals::setDefaultHandler(void (*handler)())
{
    serialCommandHandler.SetDefaultHandler(handler);
}

void megUnoLinkConnectorInternals::unknownCommand()
{
    Serial.println(F("Unknown command received. Please try again."));
}

