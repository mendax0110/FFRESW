#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>

/// @brief Temperature sensor class \class TemperatureSensor
class TemperatureSensor
{
public:
    TemperatureSensor();
    ~TemperatureSensor();

    void initialize();
    float readTemperature();

    bool isInitialized() const;

private:
    bool _temperatureSensorInitialized;
    static const int TEMP_SENSOR_PIN = A0;
    static const int TEMP_SENSOR_PIN_DIG = 4;
    
    float readAnalogSensor(int pin);
    float readDigitalSensor(int pin);
    void reportError(const char* errorMessage);
};

#endif // TEMPERATURESENSOR_H
