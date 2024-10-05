#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>

/// @brief Pressure sensor class \class PressureSensor
class PressureSensor
{
public:
    PressureSensor();
    ~PressureSensor();

    void initialize();
    float readPressure();

    bool isInitialized() const;

private:
    bool _pressureSensorInitialized;
    static const int PRESSURE_SENSOR_PIN = 0;

    float readAnalogSensor(int pin);
    void reportError(const char* errorMessage);
};

#endif // PRESSURESENSOR_H
