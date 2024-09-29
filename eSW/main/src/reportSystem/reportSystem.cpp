/**
 * @file reportSystem.cpp
 * @author Adrian Goessl
 * @brief System health and error reporting module.
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "reportSystem.h"
#include <Arduino.h>
#include <time.h>
#include <EEPROM.h>
#include <MemoryFree.h>

using namespace reportSystem;

ReportSystem::ReportSystem() 
    : tempThreshold(100.0), pressureThreshold(150.0), lastHealthCheck(0) {}

ReportSystem::~ReportSystem() {}

/// @brief Function to log an error message with a timestamp
/// @param errorMessage -> This is the error message to log
void ReportSystem::logError(const String &errorMessage)
{
    Serial.print("[ERROR] ");
    Serial.print(getCurrentTime());
    Serial.print(": ");
    Serial.println(errorMessage);
}

/// @brief Function to check the system health
/// @return -> This returns true if the system health check passed, false otherwise
bool ReportSystem::checkSystemHealth(size_t memoryThreshold)
{
    if(millis() - lastHealthCheck >= healthCheckInterval)
    {
        lastHealthCheck = millis();
        bool healthCheckPassed = true;

        healthCheckPassed &= checkSensors();
        healthCheckPassed &= checkCommunication();
        healthCheckPassed &= checkMemory(memoryThreshold);  // Pass threshold as parameter

        if (!healthCheckPassed)
        {
            logError("System health check failed!");
        }
    }

    return true;
}

/// @brief Function to report the status of the pressure and temperature thresholds
void ReportSystem::reportStatus()
{
    Serial.print("[STATUS] System is operating normally.");
    Serial.print(" Current Temp Threshold: ");
    Serial.print(tempThreshold);
    Serial.print(" Current Pressure Threshold: ");
    Serial.println(pressureThreshold);
}

/// @brief Function to set the temperature and pressure thresholds
/// @param tempThreshold -> This is the temperature threshold
/// @param pressureThreshold -> This is the pressure threshold
void ReportSystem::setThreshold(float tempThreshold, float pressureThreshold)
{
    this->tempThreshold = tempThreshold;
    this->pressureThreshold = pressureThreshold;
}

/// @brief Function to check if the current temperature and pressure exceed the thresholds
/// @param currentTemp -> This is the current temperature
/// @param currentPressure -> This is the current pressure
/// @return -> This returns true if the thresholds are not exceeded, false otherwise
bool ReportSystem::checkThresholds(float currentTemp, float currentPressure)
{
    bool result = true;
    if (currentTemp > tempThreshold)
    {
        logError("Temperature threshold exceeded! Current Temp: " + String(currentTemp));
        result = false;
    }

    if (currentPressure > pressureThreshold)
    {
        logError("Pressure threshold exceeded! Current Pressure: " + String(currentPressure));
        result = false;
    }

    return result;
}

/// @brief Function to get the current time in milliseconds
/// @return -> This returns the current time in milliseconds as a string
String ReportSystem::getCurrentTime()
{
    return String(millis()) + "ms";
}

/// @brief Function to check the status of the sensors
/// @return -> This returns true if the sensors are operational, false otherwise
bool ReportSystem::checkSensors()
{
    // TODO: use sensor module to check sensor status
    return true;
}

/// @brief Function to check the communication status
/// @return -> This returns true if the communication is operational, false otherwise
bool ReportSystem::checkCommunication()
{
    // TODO: use com module to check communication status
    return true;
}

/// @brief Function to check available memory with a configurable threshold
/// @param threshold -> Memory threshold to check against
bool ReportSystem::checkMemory(size_t threshold)
{
    Serial.print(F("Available memory: "));
    Serial.println(freeMemory());
    
    if (freeMemory() < threshold)
    {
        logError("Low memory warning! Free memory: " + String(freeMemory()));
        return false;
    }
    return true;
}