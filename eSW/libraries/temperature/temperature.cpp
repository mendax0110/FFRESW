/**
 * @file temperature.cpp
 * @brief Implementation of the temperature class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
#include <temperature.h>

TemperatureSensor::TemperatureSensor()
    : _temperatureSensorInitialized(false)
{
}

TemperatureSensor::~TemperatureSensor()
{

}

void TemperatureSensor::initialize()
{
    _temperatureSensorInitialized = true;
    Serial.println(F("Temperature sensor initialized."));
}

float TemperatureSensor::readTemperature()
{
    if (!_temperatureSensorInitialized)
    {
        reportError("Temperature sensor not initialized!");
        return NAN;
    }

    //float sensorValue = readAnalogSensor(TEMP_SENSOR_PIN);
    //float voltage = sensorValue * (5.0 / 1023.0);
    //float temperature = (voltage - 0.5) * 100.0;
    float temperature = readDigitalSensor(TEMP_SENSOR_PIN_DIG);
    return temperature;
}

float TemperatureSensor::readDht11()
{
    if (!_temperatureSensorInitialized)
    {
        reportError("Temperature sensor not initialized!");
        return NAN;
    }

    float temp = DHT.read11(DHT11_PIN);
    float temperature = DHT.temperature;
    return temperature;
}

float TemperatureSensor::readMLX90614(int choice)
{
	// init the sensor
    if (!mlx.begin())
	{
	    reportError(" [ERROR] Could not detect MLX90614!");
    }
    else
    {
    	_temperatureSensorInitialized = true;
    }

    float generalTemp = 0;

	if(choice == 1)
	{
	    // get the ambient temp
		generalTemp = mlx.readAmbientTempC();
	}

	if(choice == 2)
	{
	    // get the object temp
		generalTemp = mlx.readObjectTempC();
    }
	return generalTemp;
}

bool TemperatureSensor::isInitialized() const
{
    return _temperatureSensorInitialized;
}

float TemperatureSensor::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return (float)rawValue;
}

float TemperatureSensor::readDigitalSensor(int pin)
{
	int rawValue = digitalRead(pin);
	return (float)rawValue;
}

void TemperatureSensor::reportError(const char* errorMessage)
{
    Serial.print(F("[ERROR] "));
    Serial.println(errorMessage);
}
