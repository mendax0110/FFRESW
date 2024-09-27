/**
 * @file sensorModule.cpp
 * @author Adrian Goessl
 * @brief Implementation of sensorModule for managing various sensors.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "includes/sensorModule.h"

using namespace sensorModule;

sensorModuleInternals::sensorModuleInternals()
    : _sensorAddress(0), _sensorInitialized(false)
{
}

sensorModuleInternals::~sensorModuleInternals() {}

void sensorModuleInternals::begin()
{

}

float sensorModuleInternals::readTemperature()
{
    if (!_sensorInitialized)
    {
        Serial.println(F("Sensor not initialized!"));
        return NAN;
    }
    float temperature = 0.0f;
    return temperature;
}

float sensorModuleInternals::readPressure()
{
    if (!_sensorInitialized)
    {
        Serial.println(F("Sensor not initialized!"));
        return NAN;
    }
    float pressure = 0.0f;
    return pressure;
}

float sensorModuleInternals::readSensorData(const String& sensorType)
{
    if (sensorType.equalsIgnoreCase("temperature"))
    {
        return readTemperature();
    }
    else if (sensorType.equalsIgnoreCase("pressure"))
    {
        return readPressure();
    }
    else
    {
        Serial.println(F("Unknown sensor type!"));
        return NAN;
    }
}

void sensorModuleInternals::calibrateSensor()
{
    Serial.println(F("Calibrating sensor..."));
}

bool sensorModuleInternals::checkSensorStatus()
{
    return _sensorInitialized;
}
