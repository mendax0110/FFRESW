#include "sensorModule.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <serialMenu.h>

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
    //Serial.println(F("[INFO] Starting sensor module..."));
    SerialMenu::printToSerial(F("[INFO] Starting sensor module..."));
	initializeSensors();
}

void SensorModuleInternals::initializeSensors()
{
    Wire.begin();
    //Serial.println(F("[INFO] I2C bus initialized."));
    SerialMenu::printToSerial(F("[INFO] I2C bus initialized."));

    Wire.beginTransmission(I2C_SENSOR_ADDRESS);
    if (Wire.endTransmission() == 0)
    {
        _i2cSensorInitialized = true;
        //Serial.println(F("[INFO] I2C sensor initialized."));
        SerialMenu::printToSerial(F("[INFO] I2C sensor initialized."));
    }
    else
    {
    	_i2cSensorInitialized = false;
        //reportError("Failed to initialize I2C sensor.");
        SerialMenu::printToSerial(F("[ERROR] Failed to initialize I2C sensor."));
    }

    SPI.begin();
    pinMode(SPI_CS_PIN, OUTPUT);
    digitalWrite(SPI_CS_PIN, HIGH);
    _spiSensorInitialized = true;
    //Serial.println(F("[INFO] SPI sensor initialized."));
    SerialMenu::printToSerial(F("[INFO] SPI sensor initialized."));
    
    _pressureSensor.initialize();
    _temperatureSensor.initialize();

	/*Serial.println(F("[INFO] Checking I2C bus ..."));

	com.getI2C().beginI2C(0x07);

	if (com.getI2C().isInitialized())
	{
		reportError("[ERROR] I2C bus is not initialized!");
		return;
	}

	uint8_t testByte = 0;

	if (com.getI2C().i2cRead(I2C_SENSOR_ADDRESS, &testByte, 1) > 0)
	{
		_i2cSensorInitialized = true;
		Serial.println(F("[INFO] I2C sensor initialized."));
	}
	else
	{
		_i2cSensorInitialized = false;
		reportError("[ERROR] Failed to communicate with I2C sensor!");
	}

	Serial.println(F("[INFO] Checking SPI bus ..."));

	com.getSPI().beginSPI();

	pinMode(SPI_CS_PIN, OUTPUT);
	digitalWrite(SPI_CS_PIN, LOW);

	uint8_t handshakeCmd = 0x01;
	uint8_t handshakeResponse = 0;
	#define EXPECTED_HANDSHAKE_RESPONSE 0x01

	com.getSPI().spiWrite(&handshakeCmd, 1);
	com.getSPI().spiWrite(&handshakeResponse, 1);

	digitalWrite(SPI_CS_PIN, HIGH);

	if (handshakeResponse == EXPECTED_HANDSHAKE_RESPONSE)
	{
        _spiSensorInitialized = true;
        Serial.println(F("[INFO] SPI sensor initialized."));
	}
	else
	{
		_spiSensorInitialized = false;
	    reportError("[ERROR] SPI handshake failed.");
	}

	_spiSensorInitialized = true;

	_pressureSensor.isInitialized();
	_temperatureSensor.isInitialized();*/
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
            //reportError("Unknown sensor type.");
        	SerialMenu::printToSerial(F("ERROR Unknown sensor type."));
            return -1.0; // ERROR!!!
    }
}

float SensorModuleInternals::readI2CSensor()
{
    if (!_i2cSensorInitialized)
    {
        //reportError("I2C sensor not initialized!");
    	SerialMenu::printToSerial(F("ERROR I2C sensor not initialized!"));
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
        //reportError("Failed to read I2C sensor data!");
    	SerialMenu::printToSerial(F("[ERROR] Failed to read I2C sensor data!"));
        return NAN;
    }
}

float SensorModuleInternals::readSPISensor()
{
    if (!_spiSensorInitialized)
    {
    	SerialMenu::printToSerial(F("[ERROR] SPI sensor not initialized!"));
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
        //Serial.println(F("[INFO] Calibrating temperature sensor..."));
        SerialMenu::printToSerial(F("[INFO] Calibrating temperature sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::PRESSURE:
        //Serial.println(F("[INFO] Calibrating pressure sensor..."));
        SerialMenu::printToSerial(F("[INFO] Calibrating pressure sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::I2C_SENSOR:
        //Serial.println(F("[INFO] Calibrating I2C sensor..."));
        SerialMenu::printToSerial(F("[INFO] Calibrating I2C sensor..."));
        // TODO: add actual calibration here
        return true;
    case SensorType::SPI_SENSOR:
        //Serial.println(F("[INFO] Calibrating SPI sensor..."));
        SerialMenu::printToSerial(F("[INFO] Calibrating SPI sensor..."));
        // TODO: add actual calibration here
        return true;
    default:
        //reportError("Unknown sensor type for calibration.");
        SerialMenu::printToSerial(F("ERROR Unknown sensor type for calibration."));
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
        //reportError("Unknown sensor type for status check.");
        SerialMenu::printToSerial(F("ERROR Unknown sensor type for status check.")); // CHECK IF PRINTTOSREIAL ONLY IN MAIN:CPP or in ETHH.cpp, temp.cpp pres.cpp
        return false;
    }
}

void SensorModuleInternals::reportError(const char* errorMessage)
{
    Serial.println("[ERROR] " + String(errorMessage));
}
