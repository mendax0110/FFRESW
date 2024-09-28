/**
 * @file MegUnoLinkConnector.h
 * @brief Header file for the MegUnoLinkConnector class.
 * @version 0.1
 * @date 2024-09-27
 * 
 * This class manages serial communication using the MegunoLink framework, 
 * allowing for command handling and processing of incoming commands.
 */

#ifndef MEGUNOLINKCONNECTOR_H
#define MEGUNOLINKCONNECTOR_H

#include <Arduino.h>
#include "CommandHandler.h"

/// @brief This is the main namespace for the megUnoLinkConnector
namespace megUnoLinkConnector
{
    /// @brief Class for managing serial communication with MegunoLink.
    class megUnoLinkConnectorInternals
    {
    public:
        megUnoLinkConnectorInternals();
        ~megUnoLinkConnectorInternals();
        void begin(long baudRate);
        void processCommands();
        void addCommand(const __FlashStringHelper* command, void (*handler)(CommandParameter&));
        void setDefaultHandler(void (*handler)());

    private:
        CommandHandler<> serialCommandHandler;

        void unknownCommand();
        
    protected:
    };

} // namespace megUnoLinkConnector

#endif // MEGUNOLINKCONNECTOR_H
