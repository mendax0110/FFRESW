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
#include <ErriezMemoryUsage.h>
#include <frt.h>

using namespace reportSystem;

extern frt::Mutex serialMutex;
volatile uint16_t stackCheck __attribute__((section(".noinit")));

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
/// @param memorySizeThreshold -> Set true to check memory Threshold
/// @param checkEth -> Set true to check Ethernet
/// @param checkSpi -> Set true to check SPI
/// @param checkI2c -> Set true to check I2C
/// @param checkTemp -> Set true to check Temperature
/// @param checkPress-> Set true to check Pressure
/// @return -> This returns true if the system health check passed, false otherwise
bool ReportSystem::checkSystemHealth(size_t memoryThreshold, bool checkEth,
									bool checkSpi, bool checkI2c,
									bool checkTemp, bool checkPress)
{
    if (millis() - lastHealthCheck >= healthCheckInterval)
    {
        lastHealthCheck = millis();
        bool healthCheckPassed = true;
        String errorMsg = "System health check failed: ";

        if (!checkSensors(checkTemp, checkPress))
        {
            healthCheckPassed = false;
            errorMsg += "[Sensors Failed] ";
        }

        if (!checkCommunication(checkEth, checkSpi, checkI2c))
        {
            healthCheckPassed = false;
            errorMsg += "[Communication Failed] ";
        }

        if (!checkMemory(memoryThreshold))
        {
            healthCheckPassed = false;
            errorMsg += "[Memory Low] ";
        }

        if (detectStackOverflow())
        {
        	healthCheckPassed = false;
        	errorMsg += "[Stackoverflow] ";
        }

        if (!healthCheckPassed)
        {
            logError(errorMsg.c_str());
        }
    }

    return true;
}

/// @brief Function to report the status of the pressure and temperature thresholds
/// @param active -> Set to true to get the status, false otherwise
/// @return -> String with the Information about the status
String ReportSystem::reportStatus(bool active)
{
    if (!active)
    {
    	return "[INFO] Status Report is deactivated!";
    }

    String statusReport = "[STATUS] System is operating normally.\n";
    statusReport += "Current Temp Threshold: " + String(tempThreshold) + "\n";
    statusReport += "Current Pressure Threshold: " + String(pressureThreshold) + "\n";
    statusReport += getMemoryStatus();

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
    	logError(("Temperature threshold exceeded! Current Temperature: " + String(currentTemp)).c_str());
        result = false;
    }

    if (currentPressure > pressureThreshold)
    {
        logError(("Pressure threshold exceeded! Current Pressure:" + String(currentPressure)).c_str());
        result = false;
    }

    return result;
}

/// @brief Function to get the current time in milliseconds
/// @return -> This returns the current time in milliseconds as a string
String ReportSystem::getCurrentTime()
{
    return String(millis()) + " ms";
}

/// @brief Function to check the status of the sensors
/// @param chekTemp -> Set true to check temperature
/// @param checkPress -> Set true to check pressure
/// @return -> This returns true if the sensors are operational, false otherwise
bool ReportSystem::checkSensors(bool checkTemp, bool checkPress)
{
    // TODO: use sensor module to check sensor status
	bool tempStat = checkTemp ? _sens->checkSensorStatus(sensorModule::SensorType::TEMPERATURE) : true;
	bool pressStat = checkPress ? _sens->checkSensorStatus(sensorModule::SensorType::PRESSURE) : true;

	if (tempStat && pressStat)
	{
		return true;
	}

	String errorMsg = "Sensors failed because: ";
	if (!tempStat) errorMsg += "[Check Temp Sensor failed] ";
	if (!pressStat) errorMsg += "[Check Press Sensor failed] ";

	logError(errorMsg.c_str());
	return false;
}

/// @brief Function to check the communication status
/// @param checkEth -> Set true to check Ethernet
/// @param checkSpi -> Set true to check SPI
/// @param checkI2c -> Set true to check I2C
/// @return -> Returns true if the selected communication checks pass, false otherwise
bool ReportSystem::checkCommunication(bool checkEth, bool checkSpi, bool checkI2c)
{
    bool ethStat = checkEth ? _com->eth.isInitialized() : true;
    bool spiStat = checkSpi ? _com->spi.isInitialized() : true;
    bool i2cStat = checkI2c ? _com->i2c.isInitialized() : true;

    if (ethStat && spiStat && i2cStat)
    {
        return true;
    }

    String errorMsg = "Communication failed because: ";
    if (checkEth && !ethStat) errorMsg += "[Ethernet Not Initialized] ";
    if (checkSpi && !spiStat) errorMsg += "[SPI Not Initialized] ";
    if (checkI2c && !i2cStat) errorMsg += "[I2C Not Initialized] ";

    logError(errorMsg.c_str());
    return false;
}

/// @brief Function to check available memory with a configurable threshold
/// @param threshold -> Memory threshold to check against
bool ReportSystem::checkMemory(unsigned int threshold)
{
    if (getFreeMemSize() < threshold)
    {
        logError("Low memory warning! Free memory: " + getFreeMemSize());
        return false;
    }
    
    return true;
}

/// @brief Function to get the current memory status as a formatted string
/// @return -> A string containing details about SRAM, stack, heap, and free memory
String ReportSystem::getMemoryStatus()
{
    String memoryReport;

    memoryReport += "SRAM size: " + String(getRamSize()) + " Bytes\n";
    memoryReport += ".data size: " + String(getDataSectionSize()) + " Bytes\n";
    memoryReport += ".bss size: " + String(getBssSectionSize()) + " Bytes\n";
    memoryReport += "Stack size: " + String(getStackSize()) + " Bytes\n";
    memoryReport += "Heap size: " + String(getHeapSize()) + " Bytes\n";
    memoryReport += "Free mem: " + String(getFreeMemSize()) + " Bytes\n";

    return memoryReport;
}

/// @brief Retrieves a stack dump showing return addresses.
/// @return A string containing the stack dump.
String ReportSystem::getStackDump()
{
    uint8_t* sp = (uint8_t*)(SP);
    String stackDump = "Stack Dump (Return Addresses):\n";

    for (int i = 0; i < 10; i++)
    {
        uint16_t addr = pgm_read_word(sp);
        stackDump += "#" + String(i) + ": 0x" + String(addr, HEX) + "\n";
        sp += 2; // Move up the stack
    }

    return stackDump;
}

/// @brief Marks the start of a busy period for CPU load tracking.
void ReportSystem::startBusyTime()
{
    unsigned long currentMicros = micros();
    if (currentMicros < lastTimestamp)
    {
        // Handle overflow
        busyTime = 0;
        idleTime = 0;
    }
    busyTime += currentMicros - lastTimestamp;
    lastTimestamp = currentMicros;
}

/// @brief Marks the start of an idle period for CPU load tracking.
void ReportSystem::startIdleTime()
{
    unsigned long currentMicros = micros();
    if (currentMicros < lastTimestamp)
    {
        // Handle overflow
        busyTime = 0;
        idleTime = 0;
    }
    idleTime += currentMicros - lastTimestamp;
    lastTimestamp = currentMicros;
}

/// @brief Calculates the CPU load percentage based on busy and idle time.
/// @return The CPU load as a floating-point percentage (0.0 - 100.0).
float ReportSystem::getCPULoad()
{
    unsigned long totalTime = busyTime + idleTime;
    return (totalTime == 0) ? 0 : (busyTime * 100.0 / totalTime);
}

/// @brief Resets the CPU load tracking metrics.
void ReportSystem::resetUsage()
{
    busyTime = 0;
    idleTime = 0;
    lastTimestamp = micros();
}

/// @brief Initializer for Stack Guard
void ReportSystem::initStackGuard()
{
	stackCheck = STACK_GUARD;
}

/// @brief Detector for stackoverflow
bool ReportSystem::detectStackOverflow()
{
	return stackCheck != STACK_GUARD;
}

/// @brief Function to save last error to EEPROM
/// @param error -> Error message to save
/// HINT: KEEP IN MIND ~100 000 write cycles per cell!
void ReportSystem::saveLastError(const char* error)
{
	EEPROM.put(EEPROM_ERROR_ADDR, error);
}

/// @brief Function to get last error from EEPROM
/// HINT: KEEP IN MIND ~100 000 write cycles per cell!
String ReportSystem::getLastError()
{
	char error[50];
	EEPROM.get(EEPROM_ERROR_ADDR, error);
	return String(error);
}
