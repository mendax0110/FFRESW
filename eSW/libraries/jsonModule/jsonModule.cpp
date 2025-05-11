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
#include <jsonModule.h>
#include <serialMenu.h>

using namespace jsonModule;

JsonModuleInternals::JsonModuleInternals() : jsonBuffer(512)
{

}

JsonModuleInternals::~JsonModuleInternals()
{
	clearJson();
}

void JsonModuleInternals::createJson(const char* key, const char* value)
{
    jsonDoc[key] = value;
}

void JsonModuleInternals::createJsonFloat(const char* key, float value)
{
    jsonDoc[key] = value;
}

void JsonModuleInternals::createJsonInt(const char* key, int value)
{
    jsonDoc[key] = value;
}

void JsonModuleInternals::createJsonString(const char* key, String& value)
{
	jsonDoc[key] = value;
}

void JsonModuleInternals::createJsonStringConst(const char* key, const String& value)
{
	jsonDoc[key] = value;
}

String JsonModuleInternals::getJsonString() const
{
    String output;
    if (jsonDoc.size() == 0)
    {
       SerialMenu::printToSerial("[ERROR] JSON document is empty.");
    }
    else
    {
        serializeJson(jsonDoc, output);
    }
    return output;
}

void JsonModuleInternals::sendJsonSerial()
{
    //Serial.println(getJsonString());
    SerialMenu::printToSerial(getJsonString());
}

void JsonModuleInternals::clearJson()
{
	printJsonDocMemory();
    jsonDoc.clear();
}

void JsonModuleInternals::printJsonDocMemory()
{
	String jsonDocMem;
	jsonDocMem += "Memory usage of jsonDoc:";
	jsonDocMem += " Capacity: ";
	jsonDocMem += jsonDoc.capacity();
	jsonDocMem += " Memory Usage: ";
	jsonDocMem += jsonDoc.memoryUsage();
	jsonDocMem += " Overflowed: ";
	jsonDocMem += jsonDoc.overflowed() ? F("Yes") : F("No");
	SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, jsonDocMem);
}
