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
#include "ptrUtils.h"
#include <Arduino.h>
#include <time.h>
#include <EEPROM.h>
#include <MemoryFree.h>
#include <frt.h>

using namespace reportSystem;

extern frt::Mutex serialMutex;

ReportSystem::ReportSystem() 
    : tempThreshold(100.0), pressureThreshold(150.0), lastHealthCheck(0), _com(nullptr), _sens(nullptr)
{

}

ReportSystem::~ReportSystem()
{
	tryDeletePtr(_sens)
	tryDeletePtr(_com)
}

/// @brief Function to log an error message with a timestamp
/// @param errorMessage -> This is the error message to log
void ReportSystem::logError(const char* errorMessage)
{
    serialMutex.lock();
    Serial.print("[ERROR] ");
    Serial.print(getCurrentTime());
    Serial.print(": ");
    Serial.println(errorMessage);
    serialMutex.unlock();
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
/// @return -> String with the Information about the status
String ReportSystem::reportStatus()
{
    String statusReport = "[STATUS] System is operating normally.";
    statusReport += " Current Temp Threshold: ";
    statusReport += tempThreshold;
    statusReport += " Current Pressure Threshold: ";
    statusReport += pressureThreshold;

    return statusReport;
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
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Temperature threshold exceeded! Current Temp: %.2f", currentTemp);
        logError(buffer);
        result = false;
    }

    if (currentPressure > pressureThreshold)
    {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Pressure threshold exceeded! Current Pressure: %.2f", currentPressure);
        logError(buffer);
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
	bool statusTemp = _sens->checkSensorStatus(sensorModule::SensorType::TEMPERATURE);
	//bool statusPress = _sens->checkSensorStatus(sensorModule::SensorType::PRESSURE);

	if (statusTemp /*|| statusPress*/)
	{
		return true;
	}
	logError("temp or press sensor not init");
	return false;
}

/// @brief Function to check the communication status
/// @return -> This returns true if the communication is operational, false otherwise
bool ReportSystem::checkCommunication()
{
    // TODO: use com module to check communication status
	bool status = _com->eth.isInitialized();
	if (status)
	{
		return true;
	}
	logError("eth init failed!");
    return false;
}

/// @brief Function to check available memory with a configurable threshold
/// @param threshold -> Memory threshold to check against
bool ReportSystem::checkMemory(size_t threshold)
{
    size_t freeMem;
    freeMem = freeMemory();
    Serial.print(F("Available memory: "));
    Serial.println(freeMem);
    
    if (freeMemory() < threshold)
    {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Low memory warning! Free memory: %d", (int)freeMemory());
        logError(buffer);
        serialMutex.unlock();
        return false;
    }
    
    serialMutex.unlock();
    return true;
}
