#include <temperature.h>

TemperatureSensor::TemperatureSensor()
    : _temperatureSensorInitialized(false)
{
}

TemperatureSensor::~TemperatureSensor() {}

/// @brief Initializes the temperature sensor
void TemperatureSensor::initialize()
{
    _temperatureSensorInitialized = true;
    Serial.println(F("Temperature sensor initialized."));
}

/// @brief Function to read the temperature sensor
/// @return float -> The temperature value
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

/// @brief Function to read the DHT11 sensor
/// @return temperature using DHT11 sen
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

/// @brief Function to read the MLX90614 sensor
/// @return temperature using MLX90614 methods
float TemperatureSensor::readMLX90614(int choice)
{
	// init the sensor
    if (!mlx.begin())
	{
	    reportError("MLX90614 konnte nicht gefunden werden. Bitte Sensor überprüfen!");
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

/// @brief Function to check if the temperature sensor is initialized
/// @return bool -> True if the temperature sensor is initialized, false otherwise
bool TemperatureSensor::isInitialized() const
{
    return _temperatureSensorInitialized;
}

/// @brief Function to read the analog sensor
/// @param pin -> The pin to read the sensor from
/// @return float -> The sensor value
float TemperatureSensor::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return (float)rawValue;
}

/// @brief Function to read the digital sensor
/// @param pin -> The pin to read the sensor from
/// @return float -> The sensor value
float TemperatureSensor::readDigitalSensor(int pin)
{
	int rawValue = digitalRead(pin);
	return (float)rawValue;
}

/// @brief Function to report an error
/// @param errorMessage -> The error message to report
void TemperatureSensor::reportError(const char* errorMessage)
{
    Serial.print(F("Error: "));
    Serial.println(errorMessage);
}
