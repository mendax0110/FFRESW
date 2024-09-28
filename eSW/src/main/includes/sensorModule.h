/**
 * @file sensorModule.h
 * @author your name (you@domain.com)
 * @brief Header file for managing various sensors.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

/// @brief This is the namespace for the sensor module \namespace sensorModule
namespace sensorModule
{
    /// @brief This is the class for the sensor module \class sensorModule
    class sensorModuleInternals
    {
    public:
        sensorModuleInternals();
        ~sensorModuleInternals();
        
        void begin();
        float readTemperature();
        float readPressure();
        float readSensorData(const String& sensorType);
        void calibrateSensor();
        bool checkSensorStatus();

    private:
        uint8_t _sensorAddress;
        bool _sensorInitialized;

    protected:
    };
}

#endif // SENSOR_MODULE_H
