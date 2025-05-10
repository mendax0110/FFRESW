/**
 * @file pressure.cpp
 * @brief Implementation of the pressure class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
#include "pressure.h"
#include <serialMenu.h>

PressureSensor::PressureSensor()
    : _pressureSensorInitialized(false)
{

}

PressureSensor::~PressureSensor()
{

}

void PressureSensor::initialize()
{
    _pressureSensorInitialized = true;
    SerialMenu::printToSerial(F("[INFO] Pressure sensor initialized."));
}

float PressureSensor::readPressure()
{
    if (!_pressureSensorInitialized)
    {
        //reportError("Pressure sensor not initialized!");
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Pressure sensor not initialized!"));
        return NAN;
    }

    float sensorValue = readAnalogSensor(PRESSURE_SENSOR_PIN);
    float pressure = sensorValue * (5.0 / 1023.0) * 10.0;
    return pressure;
}

bool PressureSensor::isInitialized() const
{
    return _pressureSensorInitialized;
}

float PressureSensor::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return (float)rawValue;
}
