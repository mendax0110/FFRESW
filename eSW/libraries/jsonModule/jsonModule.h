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
//#include <String.h>

/// @brief Namespace for the JSON module. \namespace jsonModule
namespace jsonModule
{
    /// @brief Class for the JSON module internals. \class jsonModuleInternals
    class JsonModuleInternals
    {
    public:
        JsonModuleInternals();
        ~JsonModuleInternals();

        void createJson(const char* key, const char* value);
        void createJsonFloat(const char* key, float value);
        void createJsonInt(const char* key, int value);
        void createJsonString(const char* key, String& value);
        
        void sendJsonSerial();
        void sendJsonEthernet();

        String getJsonString() const;
        void clearJson();
        void printJsonDocMemory();

        size_t jsonBuffer;

    private:
        StaticJsonDocument<512> jsonDoc;

    };
}

#endif // JSONMODULE_H
