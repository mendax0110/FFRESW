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
#include <math.h>
#include <calcModule.h>

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
        SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Pressure sensor not initialized!"));
        return NAN;
    }

    float sensorValue = readAnalogSensor(PRESSURE_SENSOR_PIN);
    float pressure = calcModule::CalcModuleInternals::calculatePressureFromSensor(sensorValue, calcModule::PressureUnit::Bar);
    return pressure;
}

bool PressureSensor::isInitialized() const
{
    return _pressureSensorInitialized;
}

float PressureSensor::readAnalogSensor(uint8_t pin)
{
	uint8_t rawValue = analogRead(pin);
    return static_cast<float>(rawValue);
}
