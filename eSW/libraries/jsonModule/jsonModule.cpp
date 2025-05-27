/**
 * @file jsonModule.cpp
 * @brief Implementation of the jsonModule class.
 * @version 0.2
 * @date 2025-05-18
 *
 * @copyright Copyright (c) 2025
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <jsonModule.h>
#include <serialMenu.h>

using namespace jsonModule;

JsonModuleInternals::JsonModuleInternals() : jsonBufferSize(512)
{
}

JsonModuleInternals::~JsonModuleInternals()
{
    clearJson();
}

String JsonModuleInternals::getJsonString() const
{
    String output;
    if (jsonDoc.isNull() || jsonDoc.size() == 0)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("JSON document is empty."));
        return "";
    }

    if (serializeJson(jsonDoc, output) == 0)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to serialize JSON."));
    }

    return output;
}

void JsonModuleInternals::sendJsonSerial()
{
    SerialMenu::printToSerial(getJsonString());
}

void JsonModuleInternals::clearJson()
{
    printJsonDocMemory();
    jsonDoc.clear();
}

void JsonModuleInternals::printJsonDocMemory()
{
    String jsonDocMem = "Memory usage of jsonDoc:";
    jsonDocMem += " Capacity: ";
    jsonDocMem += jsonDoc.capacity();
    jsonDocMem += " Overflowed: ";
    jsonDocMem += jsonDoc.overflowed() ? "Yes" : "No";
    SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, jsonDocMem);
}

bool JsonModuleInternals::hasCapacityFor(size_t additionalSize) const
{
    return jsonDoc.capacity() - jsonDoc.memoryUsage() >= additionalSize;
}
