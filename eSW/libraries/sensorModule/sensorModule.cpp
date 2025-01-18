#include "sensorModule.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

using namespace sensorModule;

SensorModuleInternals::SensorModuleInternals()
    : _i2cSensorInitialized(false), 
      _spiSensorInitialized(false)
{
}

SensorModuleInternals::~SensorModuleInternals() {}

/// @brief Function to initialize the sensor module 
void SensorModuleInternals::beginSensor()
{
    Serial.println(F("Starting sensor module..."));
    initializeSensors();
}

/// @brief Function to initialize the sensors
void SensorModuleInternals::initializeSensors()
{
    Wire.begin();
    Serial.println(F("I2C bus initialized."));

    Wire.beginTransmission(I2C_SENSOR_ADDRESS);
    if (Wire.endTransmission() == 0)
    {
        _i2cSensorInitialized = true;
        Serial.println(F("I2C sensor initialized."));
    }
    else
    {
        reportError("Failed to initialize I2C sensor.");
    }

    SPI.begin();
    pinMode(SPI_CS_PIN, OUTPUT);
    digitalWrite(SPI_CS_PIN, HIGH);
    _spiSensorInitialized = true;
    Serial.println(F("SPI sensor initialized."));
    
    _pressureSensor.initialize();
    _temperatureSensor.initialize();
}

/// @brief Function to read the sensor data based on the sensor type
/// @param type -> This is the sensor type (enum)
/// @return float -> This returns the sensor data
float SensorModuleInternals::readSensor(SensorType type)
{
    switch (type)
    {
        case SensorType::TEMPERATURE:
        	return _temperatureSensor.readTemperature();
        case SensorType::PRESSURE:
        	return _pressureSensor.readPressure();
        case SensorType::I2C_SENSOR:
            return readI2CSensor();
        case SensorType::SPI_SENSOR:
            return readSPISensor();
        case SensorType::OBJECTTEMPERATURE:
        	return _temperatureSensor.readMLX90614(2);
        case SensorType::AMBIENTTEMPERATURE:
        	return _temperatureSensor.readMLX90614(1);
        case SensorType::DHT11:
        	return _temperatureSensor.readDht11();
        default:
            reportError("Unknown sensor type.");
            return -1.0; // ERROR!!!
    }
}

/// @brief Funtion to read the I2C sensor
/// @return float -> This returns the sensor data
float SensorModuleInternals::readI2CSensor()
{
    if (!_i2cSensorInitialized)
    {
        reportError("I2C sensor not initialized!");
        return NAN;
    }

    Wire.beginTransmission(I2C_SENSOR_ADDRESS);
    Wire.write(0xF7);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)I2C_SENSOR_ADDRESS, (uint8_t)6);
    if (Wire.available() == 6)
    {
        int32_t adc_P = (Wire.read() << 12) | (Wire.read() << 4) | (Wire.read() >> 4);
        return (float)adc_P / 256.0;
    }
    else
    {
        reportError("Failed to read I2C sensor data!");
        return NAN;
    }
}

/// @brief Function to read the SPI sensor
/// @return flaot -> This returns the sensor data
float SensorModuleInternals::readSPISensor()
{
    if (!_spiSensorInitialized)
    {
        reportError("SPI sensor not initialized!");
        return NAN;
    }
    digitalWrite(SPI_CS_PIN, LOW);
    
    uint16_t data = SPI.transfer16(0x00);
    digitalWrite(SPI_CS_PIN, HIGH);

    return (float)data;
}

/// @brief Function to calibrate the sensor based on the sensor type
/// @param type -> This is the sensor type (enum)
/// @return bool -> This returns true if the calibration was successful, false otherwise
bool SensorModuleInternals::calibrateSensor(SensorType type)
{
    switch (type)
    {
    case SensorType::TEMPERATURE:
        Serial.println(F("Calibrating temperature sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::PRESSURE:
        Serial.println(F("Calibrating pressure sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::I2C_SENSOR:
        Serial.println(F("Calibrating I2C sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::SPI_SENSOR:
        Serial.println(F("Calibrating SPI sensor..."));
        // TODO: add actual calibration here
        return true;
    default:
        reportError("Unknown sensor type for calibration.");
        return false;
    }
}

/// @brief Function to check the status of the sensor based on the sensor type
/// @param type -> This is the sensor type (enum)
/// @return bool -> This returns true if the sensor is initialized, false otherwise
bool SensorModuleInternals::checkSensorStatus(SensorType type)
{
    switch (type)
    {
    case SensorType::TEMPERATURE:
        return _temperatureSensor.isInitialized();
    case SensorType::PRESSURE:
        return _pressureSensor.isInitialized();
    case SensorType::I2C_SENSOR:
        return _i2cSensorInitialized;
    case SensorType::SPI_SENSOR:
        return _spiSensorInitialized;
    default:
        reportError("Unknown sensor type for status check.");
        return false;
    }
}

/// @brief Function to report an error message
/// @param errorMessage -> This is the error message to report
void SensorModuleInternals::reportError(const char* errorMessage)
{
    Serial.print(F("Error: "));
    Serial.println(errorMessage);
}
