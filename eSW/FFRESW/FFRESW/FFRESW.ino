#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <megUnoLinkConnector.h>
#include <comModule.h>
#include <reportSystem.h>
#include <jsonModule.h>

using namespace megUnoLinkConnector;
using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;
using namespace comModule;
using namespace jsonModule;

MegUnoLinkConnector linkConnector;
calcModuleInternals calc;
comModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;
jsonModuleInternals json;

// Buffers for sensor data
uint8_t i2cBuffer[10];
uint8_t spiBuffer[10];
char ethernetBuffer[256];

// Mutex to protect serial output
frt::Mutex serialMutex;

frt::Queue<float, 1> temperatureQueue;
frt::Mutex temperatureQueueMutex;

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
        yield();
        return true;
    }
};

// Task to monitor queue and system usage
class MonitoringTask final : public frt::Task<MonitoringTask>
{
private:
    static const int maxReadings = 10;
    float temperatureReadings[maxReadings];
    int currentIndex = 0;
    bool bufferFull = false;

public:
    bool run()
    {
        msleep(1000);
        yield();

        serialMutex.lock();
        float temperature = sens.readSensor(SensorType::DHT11);
        Serial.print(F("Current Temperature: "));
        Serial.println(temperature);
        temperatureReadings[currentIndex] = temperature;
        currentIndex = (currentIndex + 1) % maxReadings;
        if (currentIndex == 0) bufferFull = true;

        float average = calc.calculateAverage(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        Serial.print(F("Average Temperature: "));
        Serial.println(average);

        float pressure = sens.readSensor(SensorType::PRESSURE);
        Serial.print(F("Current Pressure: "));
        Serial.println(pressure);

        float maxTemp = calc.findMaximum(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        Serial.print(F("Maximum Temperature: "));
        Serial.println(maxTemp);
        serialMutex.unlock();

        serialMutex.lock();
        com.eth.handleEthernetClient();
        serialMutex.unlock();

        yield();
        return true;
    }
};

class MegunoTask final : public frt::Task<MegunoTask>
{
public:
    bool run()
    {
        msleep(1000);
        serialMutex.lock();
        linkConnector.beginMegUno(9600);
        linkConnector.addCommand(F("commandNameA"), 0);
        linkConnector.addCommand(F("commandNameB"), 0);
        linkConnector.setDefaultHandler(handleUnknownCommand);
        serialMutex.unlock();

        yield();
        return true;
    }
};

class SensorAndJsonTask final : public frt::Task<SensorAndJsonTask, 2048>
{
public:
    bool run()
    {
        msleep(2000);

        // Read temperature sensor (Ambient Temperature)
        float temperature = sens.readSensor(SensorType::AMBIENTTEMPERATURE);

        serialMutex.lock();
        Serial.print(F("Ambient Temp (C): "));
        Serial.println(temperature);
        serialMutex.unlock();

        float temperatureF = calc.celsiusToFahrenheit(temperature);

        serialMutex.lock();
        Serial.print(F("Ambient Temp (F): "));
        Serial.println(temperatureF);
        serialMutex.unlock();

        // pop if necessary and then push the new value
        temperatureQueueMutex.lock();
        serialMutex.lock();
        Serial.print(F("Queue size before push: "));
        Serial.println(temperatureQueue.getFillLevel());
        serialMutex.unlock();

        // f the queue is full, pop an item before pushing the new one
        if (temperatureQueue.getFillLevel() > 0)
        {
            float temp;
            temperatureQueue.pop(temp);
        }
        temperatureQueue.push(temperature);  //push the new temperature

        serialMutex.lock();
        Serial.print(F("Queue size after push: "));
        Serial.println(temperatureQueue.getFillLevel());
        serialMutex.unlock();
        temperatureQueueMutex.unlock();

        json.clearJson();
        json.createJson("task", "SensorAndJsonTask");
        json.createJson("status", "running");
        json.createJsonFloat("temperature", temperature);

        serialMutex.lock();
        json.printJsonDocMemory();
        serialMutex.unlock();

        serialMutex.lock();
        Serial.println(F("Attempting to generate JSON string..."));
        String jsonString = json.getJsonString();

        if (jsonString.length() > 0)
        {
            Serial.println(F("Prepared JSON:"));
            Serial.println(jsonString);
        }
        else
        {
            Serial.println(F("Error: JSON string is empty or failed to generate."));
        }
        serialMutex.unlock();

        if (com.eth.isInitialized())
        {
            com.eth.sendEthernetData(json.getJsonString().c_str());
            serialMutex.lock();
            Serial.println(F("Sent JSON data over Ethernet."));
            serialMutex.unlock();
        }
        else
        {
            serialMutex.lock();
            Serial.println(F("Ethernet not initialized."));
            serialMutex.unlock();
        }

        com.eth.handleEthernetClient();

        yield();
        return true;
    }
};

ReportTask reportTask;
MonitoringTask monitoringTask;
MegunoTask megunoTask;
SensorAndJsonTask sensorAndJsonTask;

void handleUnknownCommand()
{
    serialMutex.lock();
    Serial.println(F("Unknown command received."));
    serialMutex.unlock();
}

void setup()
{
    Serial.begin(9600);

    Serial.println(F("Starting sensor module..."));
    sens.beginSensor();
    com.i2c.beginI2C(0x78);
    com.spi.beginSPI();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.eth.beginEthernet(mac, ip);

    if (com.eth.isInitialized())
    {
        Serial.println("Ethernet is running!");
    }

    pinMode(13, OUTPUT);
    pinMode(8, OUTPUT);
    Serial.println(F("Setup complete."));

    // Start tasks
    reportTask.start(1);
    sensorAndJsonTask.start(2);
}

void loop()
{
    // No need for loop; tasks handle the work
}


