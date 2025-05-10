#ifndef LOCKER_BASE_H
#define LOCKER_BASE_H

#include <frt.h>
#include <Arduino.h>
#include <scopedLock.h>
#include <logManager.h>

/// @brief Base class for the locker system. \class LockerBase
class LockerBase
{
public:
    LockerBase()
    {
        if (_logger->isSDCardInitialized())
        {
            _logger->setLogFileName("log_LockerBase.txt");
        }
    }

    ~LockerBase() {}

    /**
     * @brief 
     * 
     * @return locker::ScopedLock 
     */
    locker::ScopedLock lockEthernetScoped()
    {
        ethernetConnected = true;
        logState("Ethernet connected", ethernetConnected);
        return locker::ScopedLock(ethernetMutex);
    }

    /**
     * @brief 
     * 
     * @return locker::ScopedLock 
     */
    locker::ScopedLock lockTemperatureScoped()
    {
        temperatureReading = true;
        logState("Temperature reading", temperatureReading);
        return locker::ScopedLock(temperaturQueueMutex);
    }

    /**
     * @brief 
     * 
     * @return locker::ScopedLock 
     */
    locker::ScopedLock lockSerialScoped()
    {
        serialReading = true;
        logState("Serial reading", serialReading);
        return locker::ScopedLock(serialMutex);
    }

private:

    // Mutexes for the different resources
    frt::Mutex temperaturQueueMutex;
    frt::Mutex ethernetMutex;
    frt::Mutex serialMutex;

    // Semaphores for the different resources
    frt::Semaphore ethernetSemaphore;
    frt::Semaphore temperatureSemaphore;
    frt::Semaphore serialSemaphore;

    // Flags for the different resources
    bool ethernetConnected = false;
    bool temperatureReading = false;
    bool serialReading = false;

    // Logger instance
    LogManager* _logger = LogManager::getInstance();

    /**
     * @brief 
     * 
     * @param label 
     * @param state 
     */
    void logState(const char* label, bool state)
    {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "[LockerBase] %s: %s", label, state ? "true" : "false");
        _logger->writeToLogFile(buffer);
    }
};

#endif // LOCKER_BASE_H