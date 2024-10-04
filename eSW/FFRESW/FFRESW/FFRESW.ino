#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <megUnoLinkConnector.h>
#include <comModule.h>
#include <reportSystem.h>

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

// Example sensor data
float sensorData[] = {25.0, 26.5, 24.8, 27.2, 23.9};
const int sensorDataLength = sizeof(sensorData) / sizeof(sensorData[0]);

// Buffers for sensor data
uint8_t i2cBuffer[10];
uint8_t spiBuffer[10];
char ethernetBuffer[256];

 // Mutex to protect serial output
frt::Mutex serialMutex;

// Task to report system health and status periodically
class ReportTask final : public frt::Task<ReportTask, 200>
{
public:
    bool run()
    {
        serialMutex.lock();
        if (!report.checkSystemHealth(1000))
        {
            Serial.println(F("Critical issue detected! Take action."));
        }
        report.reportStatus();
        serialMutex.unlock();
        msleep(2000);
        return true;
    }
};

// Task to monitor queue and system usage
class MonitoringTask final : public frt::Task<MonitoringTask>
{
public:
    bool run()
    {
        msleep(1000);
        serialMutex.lock();
        linkConnector.processCommands();
        serialMutex.unlock();
        return true;
    }
};

// Task instances
ReportTask reportTask;
MonitoringTask monitoringTask;

void handleCommandA(CommandParameter &params)
{
    serialMutex.lock();
    Serial.println(F("Command A received!"));

    float temperature = sens.readSensor(SensorType::TEMPERATURE);
    Serial.print(F("Current Temperature: "));
    Serial.println(temperature);

    // Calculate average from sensor data array
    float average = calc.calculateAverage(sensorData, sensorDataLength);
    Serial.print(F("Average Temperature: "));
    Serial.println(average);
    serialMutex.unlock();
}

void handleCommandB(CommandParameter &params)
{
    serialMutex.lock();
    Serial.println(F("Command B received!"));

    float pressure = sens.readSensor(SensorType::PRESSURE);
    Serial.print(F("Current Pressure: "));
    Serial.println(pressure);

    // Find maximum value from sensor data array
    float maxTemp = calc.findMaximum(sensorData, sensorDataLength);
    Serial.print(F("Maximum Temperature: "));
    Serial.println(maxTemp);
    serialMutex.unlock();
}

void handleUnknownCommand()
{
    serialMutex.lock();
    Serial.println(F("Unknown command received."));
    serialMutex.unlock();
}

void setup()
{
    // Set up serial and MegUnoLink
    Serial.begin(9600);
    linkConnector.beginMegUno(9600);
    linkConnector.addCommand(F("CommandA"), handleCommandA);
    linkConnector.addCommand(F("CommandB"), handleCommandB);
    linkConnector.setDefaultHandler(handleUnknownCommand);

    // Initialize sensors and communication modules
    Serial.println(F("Starting sensor module..."));
    sens.beginSensor();
    com.i2c.beginI2C(0x76);
    com.spi.beginSPI();
    com.i2c.beginI2C(0x76);
    com.spi.beginSPI();

    // Ethernet initialization
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(127, 0, 0, 1);
    com.eth.beginEthernet(mac, ip);

    pinMode(13, OUTPUT);
    Serial.println(F("Setup complete."));

    // Start tasks for reporting and monitoring
    reportTask.start(2);    // Medium priority
    monitoringTask.start(3);  // High priority
}

void loop()
{
    // No need to handle anything in the main loop as tasks take care of it
}
