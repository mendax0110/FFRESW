/**
 * @file reportSystem.h
 * @author your name (you@domain.com)
 * @brief 
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

#define STACK_GUARD 0xDEAD
extern volatile uint16_t stackCheck;

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

        void logError(const char* errorMessage);
        bool checkSystemHealth(size_t memoryThreshold, bool checkEth,
        						bool checkSpi, bool checkI2c,
								bool checkTemp, bool checkPress);
        String reportStatus(bool active);
        void setThreshold(float tempThreshold, float pressureThreshold);
        bool checkThresholds(float currentTemp, float currentPressure);

        String getCurrentTime();
        String getMemoryStatus();
        String getStackDump();

        void startBusyTime();
        void startIdleTime();
        float getCPULoad();
        void resetUsage();

        static void initStackGuard();
        static bool detectStackOverflow();

        void saveLastError(const char* error);
        String getLastError();

    private:
        float tempThreshold;
        float pressureThreshold;
        unsigned long lastHealthCheck;
        const unsigned long healthCheckInterval = 10000; // 10 seconds
        unsigned long busyTime = 0;
        unsigned long idleTime = 0;
        unsigned long lastTimestamp = 0;

        bool checkSensors(bool checkTemp, bool checkPress);
        bool checkCommunication(bool checkEth, bool checkSpi, bool checkI2c);
        bool checkMemory(unsigned int threshold);

        sensorModule::SensorModuleInternals* _sens;
        comModule::ComModuleInternals* _com;
    };
}

#endif // REPORTSYSTEM_H
