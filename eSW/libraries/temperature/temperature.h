#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>
#include <dht.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_I2CDevice.h>

/// @brief Temperature sensor class \class TemperatureSensor
class TemperatureSensor
{
public:
    TemperatureSensor();
    ~TemperatureSensor();

    void initialize();
    float readTemperature();
    float readDht11();
    float readMLX90614(int choice);

    bool isInitialized() const;

private:
    bool _temperatureSensorInitialized;
    static const int TEMP_SENSOR_PIN = A0;
    static const int TEMP_SENSOR_PIN_DIG = 4;
    static const int DHT11_PIN = 7;
    
    static const uint8_t MLX90614 = 0x5A;
    static const uint8_t AMBIENT_TEMP = 0x06;
    static const uint8_t OBJECT_TEMP = 0x07;

    Adafruit_MLX90614 mlx = Adafruit_MLX90614();

    float readAnalogSensor(int pin);
    float readDigitalSensor(int pin);
    void reportError(const char* errorMessage);

    dht DHT;
};

#endif // TEMPERATURESENSOR_H
