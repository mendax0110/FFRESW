/**
 * @file sensorModule.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SENSORMODULE_H
#define SENSORMODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "pressure/pressureSensor.h"
#include "temperature/temperatureSensor.h"

/// @brief Namespace for the sensor module. \namespace sensorModule
namespace sensorModule
{
    /// @brief Enum class for the sensor types. \enum SensorType
    enum class SensorType
    {
        TEMPERATURE,
        PRESSURE,
        I2C_SENSOR,
        SPI_SENSOR,
        UNKNOWN
    };

    /// @brief Class for the sensor module internals. \class SensorModuleInternals
    class SensorModuleInternals
    {
    public:
        SensorModuleInternals();
        ~SensorModuleInternals();

        void begin();
        float readSensor(SensorType type);
        float readSensorData(const String& sensorType);
        bool calibrateSensor(SensorType type);
        bool checkSensorStatus(SensorType type);

    private:
        TemperatureSensor _temperatureSensor;
        PressureSensor _pressureSensor;

        bool _i2cSensorInitialized;
        bool _spiSensorInitialized;

        static const uint8_t I2C_SENSOR_ADDRESS = 0x76;
        static const int SPI_CS_PIN = 10;

        void initializeSensors();
        float readI2CSensor();
        float readSPISensor();

        void reportError(const char* errorMessage);
    };
}

#endif // SENSORMODULE_H