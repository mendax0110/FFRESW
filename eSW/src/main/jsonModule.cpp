/**
 * @file jsonModule.cpp
 * @brief Implementation of the jsonModule class.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 */

#include "includes/jsonModule.h"
#include "includes/comModule.h"

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
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (float)
void jsonModuleInternals::createJson(const char* key, float value)
{
    jsonDoc[key] = value;
}

/// @brief Function to create a JSON object with a key-value pair
/// @param key -> This is the key of the JSON object (const char*)
/// @param value -> This is the value of the JSON object (int)
void jsonModuleInternals::createJson(const char* key, int value)
{
    jsonDoc[key] = value;
}

/// @brief Function to get the JSON object as a string
/// @return String -> This returns the JSON object as a string
String jsonModuleInternals::getJsonString() const
{
    String output;
    serializeJson(jsonDoc, output);
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
    comms.sendEthernetData(getJsonString().c_str());
}

/// @brief Function to clear the JSON object
void jsonModuleInternals::clearJson()
{
    jsonDoc.clear();
}
