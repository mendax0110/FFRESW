#include "pressure.h"

PressureSensor::PressureSensor()
    : _pressureSensorInitialized(false)
{
}

PressureSensor::~PressureSensor() {}

/// @brief Initializes the pressure sensor
void PressureSensor::initialize()
{
    _pressureSensorInitialized = true;
    Serial.println(F("Pressure sensor initialized."));
}

/// @brief Function to read the pressure sensor
/// @return float -> The pressure value
float PressureSensor::readPressure()
{
    if (!_pressureSensorInitialized)
    {
        reportError("Pressure sensor not initialized!");
        return NAN;
    }

    float sensorValue = readAnalogSensor(PRESSURE_SENSOR_PIN);
    float pressure = sensorValue * (5.0 / 1023.0) * 10.0;
    return pressure;
}

/// @brief Function to check if the pressure sensor is initialized
/// @return bool -> True if the pressure sensor is initialized, false otherwise
bool PressureSensor::isInitialized() const
{
    return _pressureSensorInitialized;
}

/// @brief Function to read the analog sensor
/// @param pin -> The pin to read the sensor from
/// @return float -> The sensor value
float PressureSensor::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return (float)rawValue;
}

/// @brief Function to report an error
/// @param errorMessage -> The error message to report
void PressureSensor::reportError(const char* errorMessage)
{
    Serial.print(F("Error: "));
    Serial.println(errorMessage);
}
