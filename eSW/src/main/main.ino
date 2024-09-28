/**
 * @file main.cpp
 * @brief Main file to manage program execution using MegUnoLink and sensor modules.
 * @version 0.1
 * @date 2024-01-26
 */

#include "includes/calcModule.h"
#include "includes/comModule.h"
#include "includes/sensorModule.h"
#include "includes/megUnoLinkConnector.h"
#include "includes/reportSystem.h"

using namespace megUnoLinkConnector;
using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;

MegUnoLinkConnector linkConnector;
calcModuleInternals calc;
SensorModuleInternals sens;
ReportSystem report;

// Example data for testing
float sensorData[] = {25.0, 26.5, 24.8, 27.2, 23.9};
const int sensorDataLength = sizeof(sensorData) / sizeof(sensorData[0]);

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
}
