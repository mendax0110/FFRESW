#include "serialMenu.h"
#include <ptrUtils.h>
#include <logManager.h>

using namespace timeModule;

SerialMenu::SerialMenu() : currentMenu(nullptr), menuSize(0)
{

}

SerialMenu::~SerialMenu()
{
	tryDeletePtr(currentMenu);
}

void SerialMenu::load(MenuItem* items, size_t size)
{
    currentMenu = items;
    menuSize = size;
}

void SerialMenu::show()
{
    if (currentMenu == nullptr) return;

    for (size_t i = 0; i < menuSize; i++)
    {
        Serial.print(currentMenu[i].key);
        Serial.print(": ");
        Serial.println(currentMenu[i].label);
    }
    Serial.print("Select an option: ");
}

void SerialMenu::run()
{
    if (currentMenu == nullptr || !Serial.available()) return;

    char choice = Serial.read();

    for (size_t i = 0; i < menuSize; i++)
    {
        if (currentMenu[i].key == choice)
        {
            currentMenu[i].callback();
            return;
        }
    }
}

void SerialMenu::printToSerial(OutputLevel level, const String& message, bool newLine, bool logMessage)
{
    frt::Mutex serialMutex;
    serialMutex.lock();

    String output;
    if (level != OutputLevel::PLAIN)
    {
        output.reserve(message.length() + 32);
        output += '[';
        output += getCurrentTime();
        output += "] ";

        switch(level)
        {
            case OutputLevel::DEBUG:    output += "[DEBUG] "; break;
            case OutputLevel::INFO:     output += "[INFO] "; break;
            case OutputLevel::WARNING:  output += "[WARNING] "; break;
            case OutputLevel::ERROR:    output += "[ERROR] "; break;
            case OutputLevel::CRITICAL: output += "[CRITICAL] "; break;
            case OutputLevel::STATUS:   output += "[STATUS] "; break;
            case OutputLevel::PLAIN:    break; // No prefix
        }
        output += message;
    }
    else
    {
        output = message;
    }

    if (newLine)
    {
        Serial.println(output);
    }
    else
    {
        Serial.print(output);
    }

    if (logMessage)
    {
        // TODO TEST THIS THIS IS NOT TESTED YET, EXTERNAL LOGGER TO SD CARD!
        LogManager* logger = LogManager::getInstance();

        if (logger && logger->isSDCardInitialized())
        {
        	if (level == OutputLevel::WARNING ||
        		level == OutputLevel::ERROR ||
    			level == OutputLevel::CRITICAL)
        	{
        		String toLog = output;
        		if (newLine) toLog += "\n";
        		logger->writeToLogFile(toLog);
        	}
        }
    }

    serialMutex.unlock();
}

void SerialMenu::printToSerial(OutputLevel level, const __FlashStringHelper* message, bool newLine, bool logMessage)
{
    printToSerial(level, String(message), newLine, logMessage);
}

void SerialMenu::printToSerial(const String& message, bool newLine, bool logMessage)
{
    printToSerial(OutputLevel::PLAIN, message, newLine, logMessage);
}

void SerialMenu::printToSerial(const __FlashStringHelper* message, bool newLine, bool logMessage)
{
    printToSerial(OutputLevel::PLAIN, String(message), newLine, logMessage);
}

String SerialMenu::getCurrentTime()
{
    TimeModuleInternals* timeInstance = TimeModuleInternals::getInstance();
    if (timeInstance == nullptr)
    {
        printToSerial(OutputLevel::ERROR, "Time module not initialized, using fallback time");
        return "0000-00-00T00:00:00Z";
    }
    return TimeModuleInternals::formatTimeString(timeInstance->getSystemTime());
}
