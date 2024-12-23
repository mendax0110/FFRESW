/**
 * @file jsonModule.cpp
 * @brief Implementation of the jsonModule class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include "../comModule/comModule.h"
#include "jsonModule.h"

using namespace jsonModule;

JsonModuleInternals::JsonModuleInternals()
{
    clearJson();
}

JsonModuleInternals::~JsonModuleInternals()
{

}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (const char*)
void JsonModuleInternals::createJson(const char* key, const char* value)
{
    jsonDoc[key] = value;
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (float)
void JsonModuleInternals::createJsonFloat(const char* key, float value)
{
    jsonDoc[key] = value;
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (int)
void JsonModuleInternals::createJsonInt(const char* key, int value)
{
    jsonDoc[key] = value;
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (String)
void JsonModuleInternals::createJsonString(const char* key, String& value)
{
	jsonDoc[key] = value;
}


/// @brief Function to get the JSON object as a string
/// @return String -> This returns the JSON object as a string
String JsonModuleInternals::getJsonString() const
{
    String output;
    if (jsonDoc.size() == 0)
    {
        Serial.println("JSON document is empty.");
    }
    else
    {
        serializeJson(jsonDoc, output);
    }
    return output;
}

/// @brief Function to send the JSON object over Serial
void JsonModuleInternals::sendJsonSerial()
{
    Serial.println(getJsonString());
}

/// @brief Function to send the JSON object over Ethernet
void JsonModuleInternals::sendJsonEthernet()
{
    comModule::ComModuleInternals comms;
    comms.eth.sendEthernetData(getJsonString().c_str());
}

/// @brief Function to clear the JSON object
void JsonModuleInternals::clearJson()
{
    jsonDoc.clear();
}

void JsonModuleInternals::printJsonDocMemory()
{
    Serial.println(F("[DEBUG] Memory usage of jsonDoc:"));
    Serial.print(F("  Capacity: "));
    Serial.println(jsonDoc.capacity());
    Serial.print(F("  Memory Usage: "));
    Serial.println(jsonDoc.memoryUsage());
    Serial.print(F("  Overflowed: "));
    Serial.println(jsonDoc.overflowed() ? F("Yes") : F("No"));
}
