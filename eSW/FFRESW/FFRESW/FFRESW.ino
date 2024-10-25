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
private:
	static const int maxReadings = 10;
	float temperatureReadings[maxReadings];
	int currentIndex = 0;
	bool bufferFull = false;

public:
    bool run()
    {
        msleep(1000);
        serialMutex.lock();
        //float temperature = sens.readSensor(SensorType::TEMPERATURE);
        float temperature = sens.readSensor(SensorType::DHT11);
        Serial.print(F("Current Temperature: "));
        Serial.println(temperature);

        temperatureReadings[currentIndex] = temperature;
        currentIndex = (currentIndex + 1) % maxReadings;
        if (currentIndex == 0) bufferFull = true;

        // Calculate average from sensor data array
        float average = calc.calculateAverage(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        Serial.print(F("Average Temperature: "));
        Serial.println(average);
        serialMutex.unlock();

        serialMutex.lock();
        float pressure = sens.readSensor(SensorType::PRESSURE);
        Serial.print(F("Current Pressure: "));
        Serial.println(pressure);

        // Find maximum value from sensor data array
        float maxTemp = calc.findMaximum(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        Serial.print(F("Maximum Temperature: "));
        Serial.println(maxTemp);
        serialMutex.unlock();

        serialMutex.lock();
        com.eth.handleEthernetClient();
        serialMutex.unlock();
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
		return true;
	}
};

class DomeTask final : public frt::Task<DomeTask>
{
public:
	bool run()
	{
		// MY First Task
		serialMutex.lock();
		Serial.println(F("nach lock"));
		digitalWrite(8, HIGH);
		Serial.println(F("LED HIGH"));
		msleep(1000);
		digitalWrite(8,LOW);
		Serial.println(F("LED LOW"));
		msleep(1000);
		Serial.println(F("DOMETASK ENDE"));
		serialMutex.unlock();
		return true;
	}
};

// Task instances
ReportTask reportTask;
MonitoringTask monitoringTask;
MegunoTask megunoTask;
DomeTask dometask;

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

    // Initialize sensors and communication modules
    Serial.println(F("Starting sensor module..."));
    sens.beginSensor();
    com.i2c.beginI2C(0x76);
    com.spi.beginSPI();

    // Ethernet initialization
    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.eth.beginEthernet(mac, ip);

    if (com.eth.isInitialized())
    {
    	Serial.println("ethernet is running!");
    }

    pinMode(13, OUTPUT);
    pinMode(8, OUTPUT);
    Serial.println(F("Setup complete."));

    // Start tasks for reporting and monitoring
    reportTask.start(2);    // Medium priority
    monitoringTask.start(3);  // High priority
    //megunoTask.start(3);
    //dometask.start(3);
}

void loop()
{
    // No need to handle anything in the main loop as tasks take care of it
}
