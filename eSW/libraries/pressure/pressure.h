/**
 * @file pressure.h
 * @author Adrian Goessl
 * @brief Header file for the pressure library
 * @version 0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>

/// @brief Pressure sensor class \class PressureSensor
class PressureSensor
{
public:
    PressureSensor();
    ~PressureSensor();

    /**
     * @brief Function to initialize the pressure sensor.
     *
     */
    void initialize();

    /**
     * @brief Function to read the pressure from the sensor.
     *
     * @return float -> The pressure value.
     */
    float readPressure();

    /**
     * @brief Function to check if the pressure sensor is initialized.
     *
     * @return true -> if the pressure sensor is initialized
     * @return false -> if the pressure sensor is not initialized
     */
    bool isInitialized() const;

private:
    bool _pressureSensorInitialized;
    static const int PRESSURE_SENSOR_PIN = 0;

    /**
     * @brief Function to read the analog sensor.
     *
     * @param pin -> The pin to read from.
     * @return float -> The value of the sensor.
     */
    float readAnalogSensor(uint8_t pin);

};

#endif // PRESSURESENSOR_H
