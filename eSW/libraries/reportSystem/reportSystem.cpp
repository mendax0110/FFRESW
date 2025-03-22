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
#include <serialMenu.h>

using namespace reportSystem;
using namespace timeModule;

volatile uint16_t stackCheck __attribute__((section(".noinit")));

/*ReportSystem::ReportSystem()
    : tempThreshold(100.0), pressureThreshold(150.0), lastHealthCheck(0),
	  _com(nullptr), _sens(nullptr), _time(TimeModuleInternals::getInstance())
{

}*/
ReportSystem::ReportSystem() 
    : tempThreshold(100.0), pressureThreshold(150.0), lastHealthCheck(0),
      _com(new comModule::ComModuleInternals()), // Allocate memory for `_com`
      _sens(new sensorModule::SensorModuleInternals()), // Ensure `_sens` is also allocated
      _time(TimeModuleInternals::getInstance()) // Use the time module instance
{
}


ReportSystem::~ReportSystem()
{
	tryDeletePtr(_sens)
	tryDeletePtr(_com)
	tryDeletePtr(_time)
}

void ReportSystem::reportError(const char* errorMessage)
{
	String errStr;
	errStr += "[ERROR] ";
	errStr += getCurrentTime();
	errStr += ": ";
	errStr += errorMessage;
	SerialMenu::printToSerial(errStr);
}

bool ReportSystem::checkSystemHealth(size_t memoryThreshold, bool checkEth,
									bool checkSpi, bool checkI2c,
									bool checkTemp, bool checkPress)
{
    if (millis() - lastHealthCheck >= healthCheckInterval)
    {
        lastHealthCheck = millis();
        bool healthCheckPassed = true;
        String errorMsg = "[" + getCurrentTime() + "] System health check failed: ";

        String errors = "";

        if (!checkSensors(checkTemp, checkPress))
        {
            healthCheckPassed = false;
            errors += "[Sensors Failed] ";
        }

        if (!checkCommunication(checkEth, checkSpi, checkI2c))
        {
            healthCheckPassed = false;
            errors += "[Communication Failed] ";
        }

        if (!checkMemory(memoryThreshold))
        {
            healthCheckPassed = false;
            errors += "[Memory Low] ";
        }

        if (detectStackOverflow())
        {
            healthCheckPassed = false;
            errors += "[Stack Overflow] ";
        }

        if (!healthCheckPassed)
        {
        	reportError((errorMsg + errors).c_str());
        }
    }

    return true;
}

String ReportSystem::reportStatus(bool active)
{
    if (!active)
    {
        return "[INFO] Status Report is deactivated!";
    }

    String currentTime = "[" + getCurrentTime() + "] ";
    String statusReport = currentTime + "[STATUS] System is operating normally.\n";

    statusReport += "Current Temperature Threshold: " + String(tempThreshold) + "\n";
    statusReport += "Current Pressure Threshold: " + String(pressureThreshold) + "\n";
    statusReport += getMemoryStatus();

    return statusReport;
}

void ReportSystem::setThreshold(float tempThreshold, float pressureThreshold)
{
    this->tempThreshold = tempThreshold;
    this->pressureThreshold = pressureThreshold;
}

bool ReportSystem::checkThresholds(float currentTemp, float currentPressure)
{
    bool result = true;
    if (currentTemp > tempThreshold)
    {
    	reportError(("Temperature threshold exceeded! Current Temperature: " + String(currentTemp)).c_str());
        result = false;
    }

    if (currentPressure > pressureThreshold)
    {
    	reportError(("Pressure threshold exceeded! Current Pressure:" + String(currentPressure)).c_str());
        result = false;
    }

    return result;
}

String ReportSystem::getCurrentTime()
{
	if (_time == nullptr)
		return "Error _time is nullptr";
    return TimeModuleInternals::formatTimeString(_time->getSystemTime());
}

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

	reportError(errorMsg.c_str());
	return false;
}

bool ReportSystem::checkCommunication(bool checkEth, bool checkSpi, bool checkI2c)
{
    if (!_com)
    {
        reportError("[ERROR] Communication module (_com) is not initialized.");
        return false;
    }

    bool ethStat = checkEth ? _com->getEthernet().isInitialized() : true;
    bool spiStat = checkSpi ? _com->getSPI().isInitialized() : true;
    bool i2cStat = checkI2c ? _com->getI2C().isInitialized() : true;

    if (ethStat && spiStat && i2cStat)
    {
        Serial.println("[DEBUG] All communication modules are initialized.");
        return true;
    }

    String errorMsg = "Communication failed because: ";
    if (checkEth && !ethStat) errorMsg += "[Ethernet Not Initialized] ";
    if (checkSpi && !spiStat) errorMsg += "[SPI Not Initialized] ";
    if (checkI2c && !i2cStat) errorMsg += "[I2C Not Initialized] ";

    reportError(errorMsg.c_str());
    return false;
}

bool ReportSystem::checkMemory(unsigned int threshold)
{
    if (getFreeMemSize() < threshold)
    {
    	reportError("Low memory warning! Free memory: " + getFreeMemSize());
        return false;
    }
    
    return true;
}

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

float ReportSystem::getCPULoad()
{
    unsigned long totalTime = busyTime + idleTime;
    return (totalTime == 0) ? 0 : (busyTime * 100.0 / totalTime);
}

void ReportSystem::resetUsage()
{
    busyTime = 0;
    idleTime = 0;
    lastTimestamp = micros();
}

void ReportSystem::initStackGuard()
{
	stackCheck = STACK_GUARD;
}

bool ReportSystem::detectStackOverflow()
{
	return stackCheck != STACK_GUARD;
}

void ReportSystem::saveLastError(const char* error)
{
	EEPROM.put(EEPROM_ERROR_ADDR, error);
}

String ReportSystem::getLastError()
{
	char error[50];
	EEPROM.get(EEPROM_ERROR_ADDR, error);
	return String(error);
}
