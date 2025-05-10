/**
 * @file sensorModule.h
 * @author Adrian Goessl
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
#include <pressure.h>
#include <temperature.h>
#include <comModule.h>


/// @brief Namespace for the sensor module. \namespace sensorModule
namespace sensorModule
{
    /// @brief Enum class for the sensor types. \enum SensorType
    enum class SensorType
    {
        TEMPERATURE,
		OBJECTTEMPERATURE,
		AMBIENTTEMPERATURE,
        PRESSURE,
		DHT11,
		MCP9601_Celsius,
		MCP9601_Fahrenheit,
		MCP9601_Kelvin,
        UNKNOWN
    };

    /// @brief Class for the sensor module internals. \class SensorModuleInternals
    class SensorModuleInternals : public TemperatureSensor, public PressureSensor
    {
    public:
        SensorModuleInternals();
        ~SensorModuleInternals();

        /**
         * @brief Initialize the sensors.
         *
         */
        void initialize();

        /**
         * @brief Function to read the sensor.
         *
         * @param type -> The type of the sensor to read.
         * @return float -> The value of the sensor.
         */
        float readSensor(SensorType type);

        /**
         * @brief Function to calibrate the sensor.
         *
         * @param type -> The type of the sensor to calibrate.
         * @return true -> if the sensor was calibrated successfully
         * @return false -> if the sensor was not calibrated successfully
         */
        bool calibrateSensor(SensorType type);

        /**
         * @brief Function to check the status of the sensor.
         *
         * @param type -> The type of the sensor to check.
         * @return true -> if the sensor is healthy
         * @return false -> if the sensor is not healthy
         */
        bool checkSensorStatus(SensorType type);

    private:
        TemperatureSensor _temperatureSensor;
        PressureSensor _pressureSensor;

        bool _i2cSensorInitialized;
        bool _spiSensorInitialized;
    };
}

#endif // SENSORMODULE_H
