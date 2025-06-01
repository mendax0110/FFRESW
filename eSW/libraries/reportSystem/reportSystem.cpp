/**
 * @file reportSystem.cpp
 * @author Adrian Goessl
 * @brief Unified system health and error reporting module
 * @version 0.3
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
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

ReportSystem::ReportSystem() 
    : tempThreshold(100.0), pressureThreshold(150.0), lastHealthCheck(0), // TODO CLARIFY REAL THRESHOLDS FOR TEMP AND PRESS WITH TEAM!
      busyTime(0), idleTime(0), lastTimestamp(micros()),
      _com(new comModule::ComModuleInternals()),
      _sens(new sensorModule::SensorModuleInternals()),
      _time(TimeModuleInternals::getInstance())
{

}

ReportSystem::~ReportSystem()
{
    tryDeletePtr(_sens);
    tryDeletePtr(_com);
    tryDeletePtr(_time);
}

void ReportSystem::reportError(const char* errorMessage)
{
	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, errorMessage);
}

bool ReportSystem::checkSystemHealth(size_t memoryThreshold, bool checkEth,
                                   bool checkSpi, bool checkI2c,
                                   bool checkTemp, bool checkPress)
{
    if (millis() - lastHealthCheck >= healthCheckInterval)
    {
        lastHealthCheck = millis();
        bool healthCheckPassed = true;
        String errorMsg = "System health check failed: ";
        String errors;

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

        if (!checkRamLevel(7200, 6400))
        {
            healthCheckPassed = false;
            errors += "[RAM Size Critical] ";
        }

        if (detectStackOverflow())
        {
            healthCheckPassed = false;
            errors += "[Stack Overflow] ";
        }

        if (!firstHealthCheckDone || healthCheckPassed != lastHealthCheckPassed)
        {
            firstHealthCheckDone = true;
            lastHealthCheckPassed = healthCheckPassed;
            if (!healthCheckPassed)
            {
                SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, errorMsg + errors);
            }
            else
            {
                SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System health check passed."));
            }
        }
    }
    return true;
}

bool ReportSystem::reportStatus(bool active)
{
    if (!active)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Status Report is deactivated!"));
        return false;
    }

    String statusReport;
    const float warningMargin = 0.9f;
    bool hasWarningOrError = false;

    auto currentTemp = _sens->readSensor(sensorModule::SensorType::OBJECTTEMPERATURE);
    if (currentTemp >= tempThreshold * warningMargin)
    {
        statusReport += "Current Temperature: " + String(currentTemp) +
                        " / Threshold: " + String(tempThreshold) + "\n";
        hasWarningOrError = true;
    }

    if (!checkMemory(2048))
    {
        statusReport += getMemoryStatus();
        hasWarningOrError = true;
    }

    if (hasWarningOrError)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::STATUS, statusReport);
    }

    return hasWarningOrError;
}

void ReportSystem::setThreshold(float tempThreshold, float pressureThreshold)
{
    this->tempThreshold = tempThreshold;
    this->pressureThreshold = pressureThreshold;
    SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, "Thresholds updated - Temp: " +
               String(tempThreshold) + ", Pressure: " + String(pressureThreshold));
}

bool ReportSystem::checkThresholds(float currentTemp, float currentPressure)
{
    bool result = true;

    if (currentTemp > tempThreshold)
    {
        reportError(("Temperature threshold exceeded! Current: " +
                   String(currentTemp) + " Threshold: " + String(tempThreshold)).c_str());
        result = false;
    }

    if (currentPressure > pressureThreshold)
    {
        reportError(("Pressure threshold exceeded! Current: " +
                   String(currentPressure) + " Threshold: " + String(pressureThreshold)).c_str());
        result = false;
    }

    return result;
}

String ReportSystem::getCurrentTime()
{
    if (_time == nullptr)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Time module not initialized, using fallback time"));
        return "0000-00-00T00:00:00Z";
    }
    return TimeModuleInternals::formatTimeString(_time->getSystemTime());
}

bool ReportSystem::checkSensors(bool checkTemp, bool checkPress)
{
    bool tempStat = checkTemp ? _sens->checkSensorStatus(sensorModule::SensorType::TEMPERATURE) : true;
    bool pressStat = checkPress ? _sens->checkSensorStatus(sensorModule::SensorType::PRESSURE) : true;

    if (tempStat && pressStat)
    {
        return true;
    }

    String errorMsg = "Sensor failure: ";
    if (!tempStat) errorMsg += "[Temp Sensor] ";
    if (!pressStat) errorMsg += "[Press Sensor] ";

    reportError(errorMsg.c_str());
    return false;
}

bool ReportSystem::checkCommunication(bool checkEth, bool checkSpi, bool checkI2c)
{
    static bool alreadyReportedSuccess = false;

    if (!_com)
    {
        reportError("Communication module not initialized");
        alreadyReportedSuccess = false;
        return false;
    }

    bool ethStat = checkEth ? _com->getEthernet().isInitialized() : true;
    bool spiStat = checkSpi ? _com->getSPI().isInitialized() : true;
    bool i2cStat = checkI2c ? _com->getI2C().isInitialized() : true;
    bool serStat = _com->getSerial().isInitialized();

    if (ethStat && spiStat && i2cStat && serStat)
    {
        if (!alreadyReportedSuccess)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("All communication modules online."));
            alreadyReportedSuccess = true;
        }
        return true;
    }
    else
    {
        alreadyReportedSuccess = false;

        String errorMsg = "Communication failure: ";
        if (checkEth && !ethStat) errorMsg += "[Ethernet] ";
        if (checkSpi && !spiStat) errorMsg += "[SPI] ";
        if (checkI2c && !i2cStat) errorMsg += "[I2C] ";
        if (!serStat) errorMsg += "[SER] ";

        reportError(errorMsg.c_str());
        return false;
    }
}

bool ReportSystem::checkMemory(unsigned int threshold)
{
    unsigned int freeMem = getFreeMemSize();
    if (freeMem < threshold)
    {
        reportError(("Low memory! Free: " + String(freeMem) +
                    " bytes (Threshold: " + String(threshold) + ")").c_str());
        return false;
    }
    return true;
}

bool ReportSystem::checkRamLevel(unsigned int warningThreshold, unsigned int criticalThreshold)
{
    unsigned int ramSize = getRamSize();

    if (ramSize < criticalThreshold)
    {
        reportError(("CRITICAL RAM level! Available: " + String(ramSize) +
                   " bytes (Critical: " + String(criticalThreshold) + ")").c_str());
        return false;
    }

    if (ramSize < warningThreshold)
    {
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::WARNING, "Low RAM warning! Available: " +
                   String(ramSize) + " bytes (Warning: " + String(warningThreshold) + ")");
    }

    return true;
}

String ReportSystem::getMemoryStatus()
{
    String memoryReport;
    memoryReport.reserve(256); // Pre-allocate for typical memory report

    memoryReport += "SRAM: " + String(getRamSize()) + "B\n";
    memoryReport += ".data: " + String(getDataSectionSize()) + "B\n";
    memoryReport += ".bss: " + String(getBssSectionSize()) + "B\n";

    unsigned stackSize = getStackSize();
    memoryReport += (stackSize < 1024) ? "[CRITICAL] " : "";
    memoryReport += "Stack: " + String(stackSize) + "B\n";

    unsigned heapSize = getHeapSize();
    memoryReport += (heapSize < 1024) ? "[CRITICAL] " : "";
    memoryReport += "Heap: " + String(heapSize) + "B\n";

    memoryReport += "Free: " + String(getFreeMemSize()) + "B";
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
    char buffer[50] = {0};
    strncpy(buffer, error, sizeof(buffer) - 1);
    EEPROM.put(EEPROM_ERROR_ADDR, buffer);
    SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, "Error saved to EEPROM: " + String(error));
}

String ReportSystem::getLastError()
{
    char error[50];
    EEPROM.get(EEPROM_ERROR_ADDR, error);
    return String(error);
}

bool ReportSystem::getLastErrorInfo()
{
    String lastError = getLastError();
    if (lastError.length() > 0)
    {
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, "Retrieved last error: " + lastError);
        return true;
    }
    return false;
}

bool ReportSystem::isTemperatureSensorOK() const
{
	return _sens ? _sens->checkSensorStatus(sensorModule::SensorType::TEMPERATURE) : false;
}

bool ReportSystem::isCommunicationOK() const
{
	if (PtrUtils::IsNullPtr(_com))
		return false;
	return _com->getEthernet().isInitialized() &&
				_com->getSPI().isInitialized() &&
				_com->getI2C().isInitialized() &&
				_com->getSerial().isInitialized();
}

bool ReportSystem::isMemoryOK() const
{
	unsigned int defaultThreshold = 2048;
	return getFreeMemSize() >= defaultThreshold;
}

bool ReportSystem::isRamOK() const
{
	unsigned int warningThreshold = 7200;
	return getFreeMemSize() >= warningThreshold;
}

bool ReportSystem::isStackSafe() const
{
	return stackCheck == STACK_GUARD;
}

bool ReportSystem::hasNoSavedErrors() const
{
	char error[50];
	EEPROM.get(EEPROM_ERROR_ADDR, error);
	return strlen(error) == 0;
}
