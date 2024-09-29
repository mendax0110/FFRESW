/**
 * @file main.cpp
 * @brief Main file to manage program execution using MegUnoLink and sensor modules.
 * @version 0.1
 * @date 2024-01-26
 */

#include "src/calcModule/calcModule.h"
#include "src/sensorModule/sensorModule.h"
#include "src/megUnoLinkConnector/megUnoLinkConnector.h"
#include "src/comModule/comModule.h"
#include "src/reportSystem/reportSystem.h"

using namespace megUnoLinkConnector;
using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;
using namespace comModule;

MegUnoLinkConnector linkConnector;
calcModuleInternals calc;
comModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;

// Example data for testing
float sensorData[] = {25.0, 26.5, 24.8, 27.2, 23.9};
const int sensorDataLength = sizeof(sensorData) / sizeof(sensorData[0]);

// bufer for data
uint8_t i2cBuffer[10];
uint8_t spiBuffer[10];
char ethernetBuffer[256];
char serialBuffer[256];

// Calculate and print the average temperature
void handleCommandA(CommandParameter &params)
{
    Serial.println("Command A received!");
    
    float temperature = sens.readSensor(SensorType::TEMPERATURE);
    Serial.print("Current Temperature: ");
    Serial.println(temperature);

    float average = calc.calculateAverage(sensorData, sensorDataLength);
    Serial.print("Average Temperature: ");
    Serial.println(average);
}

// Find and print the maximum temperature
void handleCommandB(CommandParameter &params)
{
    Serial.println("Command B received!");

    float pressure = sens.readSensor(SensorType::PRESSURE);
    Serial.print("Current Pressure: ");
    Serial.println(pressure);

    float maxTemp = calc.findMaximum(sensorData, sensorDataLength);
    Serial.print("Maximum Temperature: ");
    Serial.println(maxTemp);
}

// Handle unknown commands
void handleUnknownCommand()
{
    Serial.println("Unknown command received.");
}

void setup()
{
    Serial.begin(9600);
    linkConnector.begin(9600);
    
    linkConnector.addCommand(F("CommandA"), handleCommandA);
    linkConnector.addCommand(F("CommandB"), handleCommandB);
    linkConnector.setDefaultHandler(handleUnknownCommand);

    sens.begin();
    com.i2c.beginI2C(0x76);
    com.spi.beginSPI();
    
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(127, 0, 0, 1);
    com.ethernet.beginEthernet(mac, ip);

    pinMode(13, OUTPUT);
    Serial.println("Setup complete.");
}

void loop()
{
    if (!report.checkSystemHealth(1000))
    {
        Serial.println("Critical issue detected! Take action.");
    }

    report.reportStatus();

    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);

    linkConnector.processCommands();
    
    Serial.println("Reading data via I2C...");
    com.i2c.i2cRead(0x76, i2cBuffer, sizeof(i2cBuffer));
    Serial.print("I2C Data: ");
    for (size_t i = 0; i < sizeof(i2cBuffer); ++i)
    {
        Serial.print(i2cBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    Serial.println("Reading data via SPI...");
    com.spi.spiRead(spiBuffer, sizeof(spiBuffer));
    Serial.print("SPI Data: ");
    for (size_t i = 0; i < sizeof(spiBuffer); ++i)
    {
        Serial.print(spiBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    Serial.println("Receiving data via Ethernet...");
    com.ethernet.receiveEthernetData(ethernetBuffer, sizeof(ethernetBuffer) - 1);
    Serial.print("Ethernet Data: ");
    Serial.println(ethernetBuffer);
}
