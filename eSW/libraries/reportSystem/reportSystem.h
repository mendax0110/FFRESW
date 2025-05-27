/**
 * @file reportSystem.h
 * @author Adrian Goessl
 * @brief Header file for the ReportSystem
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef REPORTSYSTEM_H
#define REPORTSYSTEM_H

#include <Arduino.h>
#include "../sensorModule/sensorModule.h"
#include "../comModule/comModule.h"
#include "../timeModule/timeModule.h"

#define STACK_GUARD 0xDEAD // Stack guard value
extern volatile uint16_t stackCheck; // Stack check variable

#define EEPROM_ERROR_ADDR 0

/// @brief Namespace for the report system. \namespace reportSystem
namespace reportSystem
{
    /// @brief Class for the report system. \class ReportSystem
    class ReportSystem
    {
    public:
        ReportSystem();
        ~ReportSystem();

        /**
         * @brief Function to log an error message
         *
         * @param errorMessage -> The error message to log
         */
        void reportError(const char* errorMessage);

        /**
         * @brief Function to check the system health of the uC
         *
         * @param memoryThreshold -> The memory threshold to check
         * @param checkEth -> Check the Ethernet connection
         * @param checkSpi -> Check the SPI connection
         * @param checkI2c -> Check the I2C connection
         * @param checkTemp -> Check the temperature sensor
         * @param checkPress -> Check the pressure sensor
         * @return true -> if the system is healthy
         * @return false -> if the system is not healthy
         */
        bool checkSystemHealth(size_t memoryThreshold, bool checkEth,
        						bool checkSpi, bool checkI2c,
								bool checkTemp, bool checkPress);

        /**
         * @brief Function to report the status of the system
         *
         * @param active -> The status of the system
         * @return bool -> The status of the system
         */
        bool reportStatus(bool active);

        /**
         * @brief Set Thresholds for the pressure and temperature sensors
         *
         * @param tempThreshold -> The temperature threshold
         * @param pressureThreshold -> The pressure threshold
         */
        void setThreshold(float tempThreshold, float pressureThreshold);

        /**
         * @brief Check the thresholds for the temperature and pressure sensors
         *
         * @param currentTemp -> The current temperature
         * @param currentPressure -> The current pressure
         * @return true -> if the thresholds are met
         * @return false -> if the thresholds are not met
         */
        bool checkThresholds(float currentTemp, float currentPressure);

        /**
         * @brief Get the Current Time of the system
         *
         * @return String -> The current time
         */
        String getCurrentTime();

        /**
         * @brief Get the Memory Status of the system
         *
         * @return String -> The memory status
         */
        String getMemoryStatus();

        /**
         * @brief Get the Stack Dump of the system
         *
         * @return String -> The stack dump
         */
        String getStackDump();

        /**
         * @brief For Stack Guarding
         *
         */
        void startBusyTime();

        /**
         * @brief For Stack Guarding
         *
         */
        void startIdleTime();

        /**
         * @brief Getter for the CPU Load
         *
         * @return float -> The CPU Load
         */
        float getCPULoad();

        /**
         * @brief Start the CPU Load Calculation
         *
         */
        void resetUsage();

        /**
         * @brief Initialize the Stack Guard
         *
         */
        static void initStackGuard();

        /**
         * @brief Detect Stack Overflow
         *
         * @return true -> if the stack has overflowed
         * @return false -> if the stack has not overflowed
         */
        static bool detectStackOverflow();

        /**
         * @brief Saves last error message to EEPROM
         *
         * HINT: KEEP IN MIND ~100 000 write cycles per cell!
         * @param error -> The error message to save
         */
        void saveLastError(const char* error);

        /**
         * @brief Get the Last Error message from EEPROM
         *
         * @return String -> The last error message
         */
        String getLastError();

        /**
         * @brief Get the Last Error message from EEPROM
         *
         * @return bool -> used by the Endpoint to report to HAS
         */
        bool getLastErrorInfo();

        /**
         * @brief Function to check the SRAM level on the hostsystem
         *
         * @param warningThreshold -> first warning to get
         * @param criticalThreshold -> last warning to get
         *
         * @return true -> if the level exceeded
         * @return false -> if the levels are withing the thresholds
         */
        bool checkRamLevel(unsigned int warningThreshold, unsigned int criticalThreshold);

        /**
         * @brief Function to report to the HAS via Endpoint if TempSensor is initialized and ok
         *
         * @return True if sensor is ok, false otherwise
         */
        bool isTemperatureSensorOK() const;

        /**
         * @brief Function to report to the HAS via Endpoint if Communication is initialized and ok
         *
         * @return True if communication is up, false otherwise
         */
        bool isCommunicationOK() const;

        /**
         * @brief Function to report to the HAS via Endpoint if Memory is in between the required bounds
         *
         * @return True if memory is in between legal bounds, false otherwise
         */
        bool isMemoryOK() const;

        /**
         * @brief Function to report to the HAS via Endpoint if RAM is in between the required bounds
         *
         * @return True if RAM is in between legal bounds, false otherwise
         */
        bool isRamOK() const;

        /**
         * @brief Function to report to the HAS via Endpoint if Stack is not overflowing or has other issues
         *
         * @return True if stack if safe, false otherwise
         */
        bool isStackSafe() const;

        /**
         * @brief Function to report to the HAS via Endpoint if any errors were written to the EEPROM
         *
         * @return True if no errors in EEPROM, false otherwise
         */
        bool hasNoSavedErrors() const;

    private:
        float tempThreshold;
        float pressureThreshold;
        unsigned long lastHealthCheck;
        const unsigned long healthCheckInterval = 10000; // 10 seconds
        unsigned long busyTime = 0;
        unsigned long idleTime = 0;
        unsigned long lastTimestamp = 0;
        bool lastHealthCheckPassed = true;
        bool firstHealthCheckDone = false;

        /**
         * @brief Function to check the sensors
         *
         * @param checkTemp -> Check the temperature sensor
         * @param checkPress -> Check the pressure sensor
         * @return true -> if the sensors are healthy
         * @return false -> if the sensors are not healthy
         */
        bool checkSensors(bool checkTemp, bool checkPress);

        /**
         * @brief Function to check the communication modules
         *
         * @param checkEth -> Check the Ethernet connection
         * @param checkSpi -> Check the SPI connection
         * @param checkI2c -> Check the I2C connection
         * @return true -> if the communication modules are healthy
         * @return false -> if the communication modules are not healthy
         */
        bool checkCommunication(bool checkEth, bool checkSpi, bool checkI2c);

        /**
         * @brief Function to check the memory usage
         *
         * @param threshold -> The memory threshold
         * @return true -> if the memory usage is healthy
         * @return false -> if the memory usage is not healthy
         */
        bool checkMemory(unsigned int threshold);

        sensorModule::SensorModuleInternals* _sens;
        comModule::ComModuleInternals* _com;
        timeModule::TimeModuleInternals* _time;
    };
}

#endif // REPORTSYSTEM_H
