#include "includes/sensorModule.h"
#include <Wire.h>
#include <SPI.h>

using namespace sensorModule;

SensorModuleInternals::SensorModuleInternals()
    : _temperatureSensorInitialized(false), 
      _pressureSensorInitialized(false),
      _i2cSensorInitialized(false),
      _spiSensorInitialized(false)
{
}

SensorModuleInternals::~SensorModuleInternals() {}

/// @brief Function to initialize the sensor module 
void SensorModuleInternals::begin()
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

    _temperatureSensorInitialized = true;
    _pressureSensorInitialized = true;

    Serial.println(F("Temperature and Pressure sensors initialized."));
}

/// @brief Function to read the temperature sensor
/// @return flaot -> This returns the temperature
float SensorModuleInternals::readTemperature()
{
    if (!_temperatureSensorInitialized)
    {
        reportError("Temperature sensor not initialized!");
        return NAN;
    }

    float sensorValue = readAnalogSensor(TEMP_SENSOR_PIN);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperature = (voltage - 0.5) * 100.0;
    return temperature;
}

/// @brief Function to read the pressure sensor
/// @return flaot -> This returns the pressure
float SensorModuleInternals::readPressure()
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

/// @brief Function to read the I2C sensor
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
    
    Wire.requestFrom((uint8_t)I2C_SENSOR_ADDRESS, (uint8_t)6);  // TODO: correct type usd?
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
/// @return float -> This returns the sensor data
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

/// @brief Function to read the sensor data based on the sensor type
/// @param type -> This is the sensor type (enum)
/// @return float -> This returns the sensor data
float SensorModuleInternals::readSensor(SensorType type)
{
    switch (type) {
        case SensorType::TEMPERATURE:
            return readTemperature();
        case SensorType::PRESSURE:
            return readPressure();
        case SensorType::I2C_SENSOR:
            return readI2CSensor();
        case SensorType::SPI_SENSOR:
            return readSPISensor();
        default:
            reportError("Unknown sensor type.");
            return -1.0; // ERROR!!!
    }
}

/// @brief Function to calibrate the sensor based on the sensor type
/// @return float -> This returns the sensor data
float readTemperature()
{
    // TODO: Implement temperature reading
    return 0.0;
}

/// @brief Function to read the pressure sensor
/// @return float -> This returns the pressire
float readPressure()
{
    // TODO: Implement pressure reading
    return 0.0;
}

/// @brief Funtion to read the I2C sensor
/// @return float -> This returns the sensor data
float readI2CSensor()
{
    // TODO: Implement I2C reading
    return 0.0;
}

/// @brief Function to read the SPI sensor
/// @return flaot -> This returns the sensor data
float readSPISensor()
{
    // TODO: Implement SPI reading
    return 0.0;
}

/// @brief Function to read the sensor data based on the sensor type
/// @param sensorType -> This is the sensor type as a string
/// @return float -> This returns the sensor data
float SensorModuleInternals::readSensorData(const String& sensorType)
{
    if (sensorType.equalsIgnoreCase("temperature"))
    {
        return readTemperature();
    }
    else if (sensorType.equalsIgnoreCase("pressure"))
    {
        return readPressure();
    }
    else if (sensorType.equalsIgnoreCase("i2c"))
    {
        return readI2CSensor();
    }
    else if (sensorType.equalsIgnoreCase("spi"))
    {
        return readSPISensor();
    }
    else
    {
        reportError("Unknown sensor type!");
        return NAN;
    }
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
        return _temperatureSensorInitialized;
    case SensorType::PRESSURE:
        return _pressureSensorInitialized;
    case SensorType::I2C_SENSOR:
        return _i2cSensorInitialized;
    case SensorType::SPI_SENSOR:
        return _spiSensorInitialized;
    default:
        reportError("Unknown sensor type for status check.");
        return false;
    }
}

/// @brief Function to read an analog sensor
/// @param pin -> This is the analog pin to read from
/// @return float -> This returns the sensor value
float SensorModuleInternals::readAnalogSensor(int pin)
{
    int rawValue = analogRead(pin);
    return (float)rawValue;
}

/// @brief Function to report an error message
/// @param errorMessage -> This is the error message to report
void SensorModuleInternals::reportError(const char* errorMessage)
{
    Serial.print(F("Error: "));
    Serial.println(errorMessage);
}
