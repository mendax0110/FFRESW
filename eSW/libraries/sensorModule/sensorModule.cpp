/**
 * @file sensorModule.cpp
 * @brief Implementation of the sensorModule class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
#include "sensorModule.h"
#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <serialMenu.h>

using namespace sensorModule;

SensorModuleInternals::SensorModuleInternals()
    : _lastUnknownSensorType(SensorType::MCP9601_Celsius_Indoor)
	, _unknownSensorReported(false)
{

}

SensorModuleInternals::~SensorModuleInternals()
{

}

void SensorModuleInternals::initialize()
{
	SerialMenu::printToSerial(F("[INFO] Starting sensor module..."));

    _pressureSensor.initialize();
    _temperatureSensor.initialize();
}

float SensorModuleInternals::readSensor(SensorType type)
{
    switch (type)
    {
        case SensorType::TEMPERATURE:
        	return _temperatureSensor.readTemperature();
        case SensorType::PRESSURE:
        	return _pressureSensor.readPressure();
        case SensorType::MCP9601_Celsius_Indoor:
        	return _temperatureSensor.readMCP9601(Units::Celsius, SensorID::INDOOR);
        case SensorType::MCP9601_Fahrenheit_Indoor:
        	return _temperatureSensor.readMCP9601(Units::Fahrenheit, SensorID::INDOOR);
        case SensorType::MCP9601_Kelvin_Indoor:
        	return _temperatureSensor.readMCP9601(Units::Kelvin, SensorID::INDOOR);
        case SensorType::MCP9601_Celsius_Outdoor:
        	return _temperatureSensor.readMCP9601(Units::Celsius, SensorID::OUTDOOR);
        case SensorType::MCP9601_Fahrenheit_Outdoor:
        	return _temperatureSensor.readMCP9601(Units::Fahrenheit, SensorID::OUTDOOR);
        case SensorType::MCP9601_Kelvin_Outdoor:
        	return _temperatureSensor.readMCP9601(Units::Kelvin, SensorID::OUTDOOR);
        default:
        	reportUnknownSensorOnce(type, F("readSensor"));
            return NAN;
    }
}

bool SensorModuleInternals::calibrateSensor(SensorType type)
{
	uint8_t status;
    switch (type)
    {
        case SensorType::TEMPERATURE:
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Calibrating temperature sensor..."));
            // TODO: add actual calibration logic here
            return true;
        case SensorType::PRESSURE:
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Calibrating pressure sensor..."));
            // TODO: add actual calibration logic here
            return true;
        case SensorType::MCP9601_Celsius_Indoor:
        case SensorType::MCP9601_Fahrenheit_Indoor:
        case SensorType::MCP9601_Kelvin_Indoor:
        	status = calibMCP9601(SensorID::INDOOR);
        	return status != MCP9601_Status::MCP9601_OPENCIRCUIT && status != MCP9601_Status::MCP9601_SHORTCIRCUIT;
        case SensorType::MCP9601_Celsius_Outdoor:
        case SensorType::MCP9601_Fahrenheit_Outdoor:
        case SensorType::MCP9601_Kelvin_Outdoor:
        	status = calibMCP9601(SensorID::OUTDOOR);
        	return status != MCP9601_Status::MCP9601_OPENCIRCUIT && status != MCP9601_Status::MCP9601_SHORTCIRCUIT;
        default:
        	reportUnknownSensorOnce(type, F("calibrateSensor"));
            return false;
    }
}

bool SensorModuleInternals::checkSensorStatus(SensorType type)
{
    switch (type)
    {
    case SensorType::TEMPERATURE:
        return _temperatureSensor.isInitialized();
    case SensorType::PRESSURE:
        return _pressureSensor.isInitialized();
    default:
    	reportUnknownSensorOnce(type, F("checkSensorStatus"));
        return false;
    }
}

void SensorModuleInternals::reportUnknownSensorOnce(SensorType type, const __FlashStringHelper* context)
{

    if (!_unknownSensorReported || _lastUnknownSensorType != type)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Unknown sensor type in: "), false);
        SerialMenu::printToSerial(context);

        _lastUnknownSensorType = type;
        _unknownSensorReported = true;
    }
}
