/**
 * @file jsonModule.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef JSONMODULE_H
#define JSONMODULE_H

#include <Arduino.h>
#include <ArduinoJson.h>

/// @brief Namespace for the JSON module. \namespace jsonModule
namespace jsonModule
{
    /// @brief Class for the JSON module internals. \class jsonModuleInternals
    class jsonModuleInternals
    {
    public:
        jsonModuleInternals();
        ~jsonModuleInternals();

        void createJson(const char* key, const char* value);
        void createJson(const char* key, float value);
        void createJson(const char* key, int value);
        
        void sendJsonSerial();
        void sendJsonEthernet();

        String getJsonString() const;
        void clearJson();

    private:
        StaticJsonDocument<256> jsonDoc; // TODO: maybe change size
    };
}

#endif // JSONMODULE_H
