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

SensorModuleInternals::~SensorModuleInternals()
{

}

void SensorModuleInternals::beginSensor()
{
    Serial.println(F("[INFO] Starting sensor module..."));
    initializeSensors();
}

void SensorModuleInternals::initializeSensors()
{
    Wire.begin();
    Serial.println(F("[INFO] I2C bus initialized."));

    Wire.beginTransmission(I2C_SENSOR_ADDRESS);
    if (Wire.endTransmission() == 0)
    {
        _i2cSensorInitialized = true;
        Serial.println(F("[INFO] I2C sensor initialized."));
    }
    else
    {
    	_i2cSensorInitialized = false;
        reportError("[ERROR] Failed to initialize I2C sensor.");
    }

    SPI.begin();
    pinMode(SPI_CS_PIN, OUTPUT);
    digitalWrite(SPI_CS_PIN, HIGH);
    _spiSensorInitialized = true;
    Serial.println(F("[INFO] SPI sensor initialized."));
    
    _pressureSensor.initialize();
    _temperatureSensor.initialize();
}

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
            reportError("[ERROR] Unknown sensor type.");
            return -1.0; // ERROR!!!
    }
}

float SensorModuleInternals::readI2CSensor()
{
    if (!_i2cSensorInitialized)
    {
        reportError("[ERROR] I2C sensor not initialized!");
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
        reportError("[ERROR] Failed to read I2C sensor data!");
        return NAN;
    }
}

float SensorModuleInternals::readSPISensor()
{
    if (!_spiSensorInitialized)
    {
        reportError("[ERROR] SPI sensor not initialized!");
        return NAN;
    }
    digitalWrite(SPI_CS_PIN, LOW);
    
    uint16_t data = SPI.transfer16(0x00);
    digitalWrite(SPI_CS_PIN, HIGH);

    return (float)data;
}

bool SensorModuleInternals::calibrateSensor(SensorType type)
{
    switch (type)
    {
    case SensorType::TEMPERATURE:
        Serial.println(F("[INFO] Calibrating temperature sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::PRESSURE:
        Serial.println(F("[INFO] Calibrating pressure sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::I2C_SENSOR:
        Serial.println(F("[INFO] Calibrating I2C sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::SPI_SENSOR:
        Serial.println(F("[INFO] Calibrating SPI sensor..."));
        // TODO: add actual calibration here
        return true;
    default:
        reportError("[ERROR] Unknown sensor type for calibration.");
        return false;
    }
}

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

void SensorModuleInternals::reportError(const char* errorMessage)
{
    Serial.print(F("[ERROR] "));
    Serial.println(errorMessage);
}
