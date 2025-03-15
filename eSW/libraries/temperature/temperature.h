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

    /**
     * @brief Function to initialize the temperature sensor.
     *
     */
    void initialize();

    /**
     * @brief Function to read the temperature from the sensor.
     *
     * @return float -> The temperature value.
     */
    float readTemperature();

    /**
     * @brief Function to read from specific sensor DH11
     *
     * @return float -> The temperature value.
     */
    float readDht11();

    /**
     * @brief Function to read from specific sensor MLX90614
     *
     * @param choice -> The choice of the sensor to read from.
     * @return float -> The temperature value.
     */
    float readMLX90614(int choice);

    /**
     * @brief Check if the temperature sensor is initialized.
     *
     * @return true -> if the temperature sensor is initialized
     * @return false -> if the temperature sensor is not initialized
     */
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

    /**
     * @brief Function to read the analog sensor.
     *
     * @param pin -> The pin to read from.
     * @return float -> The value of the sensor.
     */
    float readAnalogSensor(int pin);

    /**
     * @brief Funciton to read the digital sensor.
     *
     * @param pin -> The pin to read from.
     * @return float -> The value of the sensor.
     */
    float readDigitalSensor(int pin);

    /**
     * @brief Function to report an error.
     *
     * @param errorMessage -> The error message.
     */
    void reportError(const char* errorMessage);

    dht DHT;

};

#endif // TEMPERATURESENSOR_H
