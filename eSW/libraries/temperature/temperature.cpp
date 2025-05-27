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
#include <math.h>

TemperatureSensor::TemperatureSensor()
    : _temperatureSensorInitialized(false)
{

}

TemperatureSensor::~TemperatureSensor()
{

}

void TemperatureSensor::initialize()
{
	if (!_mcp1.begin(0x66))
	{
	    SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init MCP9601 at 0x66"));
	    return;
	}

	if (!_mcp2.begin(0x67))
	{
	    SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init MCP9601 at 0x67"));
	    return;
	}

	_mcp1.setAmbientResolution(_ambientREs);
	_mcp1.setADCresolution(_mcp1.getADCresolution());
	_mcp1.setThermocoupleType(_mcp1.getThermocoupleType());
	_mcp1.setFilterCoefficient(3);

	_mcp2.setAmbientResolution(_ambientREs);
	_mcp2.setADCresolution(_mcp2.getADCresolution());
	_mcp2.setThermocoupleType(_mcp2.getThermocoupleType());
	_mcp2.setFilterCoefficient(3);

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

uint8_t TemperatureSensor::calibMCP9601(SensorID sensor)
{
	uint8_t status = (sensor == SensorID::INDOOR) ? _mcp1.getStatus() : _mcp2.getStatus();
	uint8_t calibStatus;

	switch (status)
	{
		case MCP9601_Status::MCP9601_OPENCIRCUIT:
			calibStatus = 0x10;
			break;
		case MCP9601_Status::MCP9601_SHORTCIRCUIT:
			calibStatus = 0x20;
			break;
		default:
			calibStatus = 0x00; // OK
			break;
	}

	return calibStatus;
}

float TemperatureSensor::readMCP9601(Units unit, SensorID sensor)
{
	uint8_t calibStatus = calibMCP9601(sensor);
	if (calibStatus == MCP9601_OPENCIRCUIT ||
		calibStatus == MCP9601_SHORTCIRCUIT ||
		!_temperatureSensorInitialized)
	{
		return NAN;
	}

	Adafruit_MCP9601* mcp = (sensor == SensorID::INDOOR) ? &_mcp1 : &_mcp2;

	float readings = 0.0f;

	switch (unit)
	{
	case Units::Celsius:
		readings = mcp->readThermocouple();
		break;
	case Units::Fahrenheit:
		readings = calcModule::CalcModuleInternals::celsiusToFahrenheit(mcp->readThermocouple());
		break;
	case Units::Kelvin:
		readings = calcModule::CalcModuleInternals::celsiusToKelvin(mcp->readThermocouple());
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

float TemperatureSensor::readAnalogSensor(uint8_t pin)
{
	uint8_t rawValue = analogRead(pin);
    return static_cast<float>(rawValue);
}

float TemperatureSensor::readDigitalSensor(uint8_t pin)
{
	uint8_t rawValue = digitalRead(pin);
	return static_cast<float>(rawValue);
}
