/**
 * @file temperature.h
 * @author Adrian Goessl
 * @brief Header file for the temperature library
 * @version 0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include "Adafruit_MCP9601.h"

/// @brief Enum for different units used by mehtods as paramters \enum Units
enum Units
{
	Celsius,
	Kelvin,
	Fahrenheit
};

/// @brief Enum for the different status codes of the MCP9601 sensor \enum MCP9601_Status
enum MCP9601_Status : uint8_t
{
	MCP9601_OPENCIRCUIT = 0x10,
	MCP9601_SHORTCIRCUIT = 0x20
};

// @brief Enum for the Different Sensors in different Environments \enum SensorID
enum SensorID
{
	INDOOR,	// Green Cable
	OUTDOOR // Silver Cable
};

/// @brief Temperature sensor class \class TemperatureSensor
class TemperatureSensor
{
public:
    TemperatureSensor();
    ~TemperatureSensor();

    /**
     * @brief Function to initialize the temperature sensor.
     *
     */
    void initialize();

    /**
     * @brief Function to read the temperature from the sensor.
     *
     * @return float -> The temperature value.
     */
    float readTemperature();

    /**
     * @brief Function to read form specific sensor MCP9601
     *
     * @param unit -> Choose the unit °C, F, K
     * @return -> The temperature value
     */
    float readMCP9601(Units unit, SensorID sensor);

    /**
     * @brief Check if the temperature sensor is initialized.
     *
     * @return true -> if the temperature sensor is initialized
     * @return false -> if the temperature sensor is not initialized
     */
    bool isInitialized() const;


    /**
     * @brief Method to calibrate the MCP9601 sensor, Indoor and Outdoor Env
     *
     * @return uint8_t -> the status of the calibration
     */
    uint8_t calibMCP9601(SensorID sensor);

private:
    bool _temperatureSensorInitialized;
    static const int TEMP_SENSOR_PIN = A0;
    static const int TEMP_SENSOR_PIN_DIG = 4;
    static const int DHT11_PIN = 7;
    
    static const uint8_t MLX90614 = 0x5A;
    static const uint8_t AMBIENT_TEMP = 0x06;
    static const uint8_t OBJECT_TEMP = 0x07;

    // Settings for the MCP9601 Sensor board
    Adafruit_MCP9601 _mcp1;
    Adafruit_MCP9601 _mcp2;
    Ambient_Resolution _ambientREs = RES_ZERO_POINT_0625;
    static const uint8_t MCP9601_I2C = 0x67;


    /**
     * @brief Function to read the analog sensor.
     *
     * @param pin -> The pin to read from.
     * @return float -> The value of the sensor.
     */
    float readAnalogSensor(uint8_t pin);

    /**
     * @brief Funciton to read the digital sensor.
     *
     * @param pin -> The pin to read from.
     * @return float -> The value of the sensor.
     */
    float readDigitalSensor(uint8_t pin);
};

#endif // TEMPERATURESENSOR_H
