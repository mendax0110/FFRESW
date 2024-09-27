/**
 * @file main.cpp
 * @author Adrian Goessl
 * @brief This is the main file. It is responsible for starting the program.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "includes/calcModule.h"
#include "includes/comModule.h"
#include "includes/sensorModule.h"
#include "includes/megUnoLinkConnector.h"

using namespace megUnoLinkConnector;
using namespace calcModule;
using namespace sensorModule;

megUnoLinkConnectorInternals linkConnector;
calcModuleInternals calc;
sensorModuleInternals sens;

//expample data
float sensorData[] = {25.0, 26.5, 24.8, 27.2, 23.9};
const int sensorDataLength = sizeof(sensorData) / sizeof(sensorData[0]);

void handleCommandA(CommandParameter &params)
{
    Serial.println(F("Command A received!"));
    float average = calc.calculateAverage(sensorData, sensorDataLength);
    float pressure = sens.readPressure();
    float temp = sens.readTemperature();
    Serial.print(F("Average Temperature: "));
    Serial.println(average);
}

void handleCommandB(CommandParameter &params)
{
    Serial.println(F("Command B received!"));
    float maxTemp = calc.findMaximum(sensorData, sensorDataLength);
    float sensorTest = sens.readSensorData("Temp");
    Serial.print(F("Maximum Temperature: "));
    Serial.println(maxTemp);
}

void handleUnknownCommand()
{
    Serial.println(F("Unknown command received."));
}

void setup()
{
    Serial.begin(9600);
    linkConnector.begin(9600);
    linkConnector.addCommand(F("CommandA"), handleCommandA);
    linkConnector.addCommand(F("CommandB"), handleCommandB);
    linkConnector.setDefaultHandler(handleUnknownCommand);
    Serial.println(F("Setup complete."));
}

void loop()
{
    linkConnector.processCommands();
}

int main(int argc, char const *argv[])
{
    loop();
    return 0;
}