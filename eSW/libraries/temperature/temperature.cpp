/**
 * @file temperature.cpp
 * @brief Implementation of the temperature class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
#include <temperature.h>
#include <serialMenu.h>
#include <ptrUtils.h>
#include <calcModule.h>

TemperatureSensor::TemperatureSensor()
    : _temperatureSensorInitialized(false)
{

}

TemperatureSensor::~TemperatureSensor()
{

}

void TemperatureSensor::initialize()
{
	if (!_mcp.begin())
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init MCP9601_I2C"));
		return NAN;
	}

	_mcp.setAmbientResolution(_ambientREs);
	_mcp.setADCresolution(_mcp.getADCresolution());
	_mcp.setThermocoupleType(_mcp.getThermocoupleType());
	_mcp.setFilterCoefficient(3);

    _temperatureSensorInitialized = true;
    SerialMenu::printToSerial(F("[INFO] Temperature sensor initialized."));
}

float TemperatureSensor::readTemperature()
{
    if (!_temperatureSensorInitialized)
    {
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Temperature sensor not initialized!"));
        return NAN;
    }
    return readDigitalSensor(TEMP_SENSOR_PIN_DIG);
}

uint8_t TemperatureSensor::calibMCP9601() // TODO CHECK LIBRARY AND FULLY IMPLEMENT
{
	uint8_t status = _mcp.getStatus();
	uint8_t calibStatus;

	switch (status)
	{
	case MCP9601_Status::MCP9601_OPENCIRCUIT:
		calibStatus = 0x10;
		break;
	case MCP9601_Status::MCP9601_SHORTCIRCUIT:
		calibStatus = 0x20;
		break;
	}

	return calibStatus;
}

float TemperatureSensor::readMCP9601(Units unit)
{
	if (!_mcp.begin())
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init MCP9601_I2C"));
		return NAN;
	}

	float readings = 0.0f;

	switch (unit)
	{
	case Units::Celsius:
		readings = _mcp.readThermocouple();
		break;
	case Units::Fahrenheit:
		readings = calcModule::CalcModuleInternals::celsiusToFahrenheit(_mcp.readThermocouple());
		break;
	case Units::Kelvin:
		readings = calcModule::CalcModuleInternals::celsiusToKelvin(_mcp.readThermocouple());
		break;
	default:
		break;
	}

	return readings;
}

bool TemperatureSensor::isInitialized() const
{
    return _temperatureSensorInitialized;
}

float TemperatureSensor::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return static_cast<float>(rawValue);
}

float TemperatureSensor::readDigitalSensor(int pin)
{
	int rawValue = digitalRead(pin);
	return static_cast<float>(rawValue);
}
