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

jsonModuleInternals::jsonModuleInternals()
{
    clearJson();
}

jsonModuleInternals::~jsonModuleInternals() {}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (const char*)
void jsonModuleInternals::createJson(const char* key, const char* value)
{
    jsonDoc[key] = value;
    Serial.print("Current JSON Size: ");
    Serial.println(jsonDoc.size());
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (float)
void jsonModuleInternals::createJsonFloat(const char* key, float value)
{
    jsonDoc[key] = value;
    Serial.print("Current JSON Size: ");
    Serial.println(jsonDoc.size());
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (int)
void jsonModuleInternals::createJsonInt(const char* key, int value)
{
    jsonDoc[key] = value;
    Serial.print("Current JSON Size: ");
    Serial.println(jsonDoc.size());
}

/// @brief Function to get the JSON object as a string
/// @return String -> This returns the JSON object as a string
String jsonModuleInternals::getJsonString() const
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
void jsonModuleInternals::sendJsonSerial()
{
    Serial.println(getJsonString());
}

/// @brief Function to send the JSON object over Ethernet
void jsonModuleInternals::sendJsonEthernet()
{
    comModule::comModuleInternals comms;
    comms.eth.sendEthernetData(getJsonString().c_str());
}

/// @brief Function to clear the JSON object
void jsonModuleInternals::clearJson()
{
    jsonDoc.clear();
}

void jsonModuleInternals::printJsonDocMemory()
{
    Serial.println(F("[DEBUG] Memory usage of jsonDoc:"));
    Serial.print(F("  Capacity: "));
    Serial.println(jsonDoc.capacity());
    Serial.print(F("  Memory Usage: "));
    Serial.println(jsonDoc.memoryUsage());
    Serial.print(F("  Overflowed: "));
    Serial.println(jsonDoc.overflowed() ? F("Yes") : F("No"));
}
