/**
 * @file megUnoLinkConnector.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MEGUNOLINKCONNECTOR_H
#define MEGUNOLINKCONNECTOR_H

#include <Arduino.h>
#include <CommandHandler.h>

/// @brief Namespace for the MegUnoLink Connector. \namespace megUnoLinkConnector
namespace megUnoLinkConnector
{
    /// @brief Class for the MegUnoLink Connector. \class MegUnoLinkConnector
    class MegUnoLinkConnector
    {
    public:
        MegUnoLinkConnector();
        ~MegUnoLinkConnector();
        
        void beginMegUno(long baudRate);
        void processCommands();

        template <typename HandlerType>
        void addCommand(const char* commandName, HandlerType handler);

        template <typename HandlerType>
        void addCommand(const __FlashStringHelper* commandName, HandlerType handler);

        void setDefaultHandler(void (*handler)());
        void handleUnknownCommand();
        void sendLog(const String &message);

    private:
        CommandHandler<> serialCommandHandler;
        bool initialized;
        bool checkInitialized();
    };

    template <typename HandlerType>
    void MegUnoLinkConnector::addCommand(const __FlashStringHelper* commandName, HandlerType handler)
    {
        if (checkInitialized())
        {
            serialCommandHandler.AddCommand(commandName, handler);
            Serial.print(F("Command added: "));
            Serial.println(reinterpret_cast<const char*>(commandName));
        }
        else
        {
            sendLog(F("MegUnoLink Connector not initialized, cannot add command."));
        }
    }
}

#endif // MEGUNOLINKCONNECTOR_H
