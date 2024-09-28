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

/// @brief Namespace for the report system. \namespace reportSystem
namespace reportSystem
{
    /// @brief Class for the report system. \class ReportSystem
    class ReportSystem
    {
    public:
        ReportSystem();
        ~ReportSystem();

        void logError(const String &errorMessage);
        bool checkSystemHealth(size_t memoryThreshold);
        void reportStatus();
        void setThreshold(float tempThreshold, float pressureThreshold);
        bool checkThresholds(float currentTemp, float currentPressure);

    private:
        float tempThreshold;
        float pressureThreshold;
        unsigned long lastHealthCheck;
        const unsigned long healthCheckInterval = 10000; // 10 seconds

        String getCurrentTime();
        bool checkSensors();
        bool checkCommunication();
        bool checkMemory(size_t threshold);
    };
}

#endif // REPORTSYSTEM_H