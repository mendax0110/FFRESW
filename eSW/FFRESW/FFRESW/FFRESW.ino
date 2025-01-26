#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <comModule.h>
#include <reportSystem.h>
#include <jsonModule.h>
#include <timeModule.h>

using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;
using namespace comModule;
using namespace jsonModule;
using namespace timeModule;

CalcModuleInternals calc;
ComModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;
JsonModuleInternals json;
TimeModuleInternals  time;
DateTimeStruct currentTime;

// Buffers for sensor data
uint8_t i2cBuffer[10];
uint8_t spiBuffer[10];
char ethernetBuffer[256];

// Mutex to protect serial output
frt::Mutex serialMutex;

frt::Queue<float, 1> temperatureQueue;
frt::Mutex temperatureQueueMutex;

// helper method to simplify printing
void printToSerial(const __FlashStringHelper* message, bool newLine = true)
{
    serialMutex.lock();
    if (newLine)
    {
        Serial.println(message);
    }
    else
    {
        Serial.print(message);
    }
    serialMutex.unlock();
}

// helper method to simplify printing
void printToSerial(const String& message, bool newLine = true)
{
    serialMutex.lock();
    if (newLine)
    {
        Serial.println(message);
    }
    else
    {
        Serial.print(message);
    }
    serialMutex.unlock();
}

// Task to report system health and status periodically
class ReportTask final : public frt::Task<ReportTask, 200>
{
public:
    bool run()
    {
    	String status = report.reportStatus();

        if (!report.checkSystemHealth(3000))
        {
        	printToSerial(F("Critical issue detected! Take action."));
        }
        else
        {
        	//printToSerial(status);
        }

        msleep(2000);
        yield();
        return true;
    }
};

class FlyBackTask final : public frt::Task<FlyBackTask>
{
private:
	//Define Pins
	const int potPin = A0;

public:

	// Function to set the PWM frequency by adjusting ICR1
	void setPWMFrequency(int frequency)
	{
	  // Calculate ICR1 based on desired frequency
	  ICR1 = 16000000 / frequency;
	  OCR1A = ICR1 / 2; // Set duty cycle to 50%
	}

	// Hauptlogik des Tasks
	bool run()
	{
		msleep(1000);

		serialMutex.lock();

		// Lese Potentiometerwert
		int potValue = analogRead(potPin);
		Serial.print(F("AnalogValue: "));
		Serial.println(potValue);

		// Map den Wert von 0–1023 auf 0–399 (entsprechend TOP-Wert)
		int frequency = map(potValue, 0, 1023, 1000, 25000);
		Serial.print(F("pwmValue: "));
		Serial.println(frequency);


	    // Calculate the duty cycle in percentage
	    float dutyCycle = ((float)OCR1A / 799) * 100.0;
	    Serial.print(F("Duty Cycle: "));
	    Serial.print(dutyCycle);
	    Serial.println(F("%"));

	    // Update the Timer1 frequency based on the mapped value
	    setPWMFrequency(frequency);

		serialMutex.unlock();
		return true; // Task erfolgreich ausgeführt
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

        float temperature = sens.readSensor(SensorType::DHT11);
        printToSerial(F("Current Temperature: "));
        printToSerial(String(temperature));

        temperatureReadings[currentIndex] = temperature;
        currentIndex = (currentIndex + 1) % maxReadings;
        if (currentIndex == 0) bufferFull = true;

        float average = calc.calculateAverage(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        printToSerial(F("Average Temperature: "));
        printToSerial(String(average));

        float pressure = sens.readSensor(SensorType::PRESSURE);
        printToSerial(F("Current Pressure: "));
        printToSerial(String(pressure));

        float maxTemp = calc.findMaximum(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        printToSerial(F("Maximum Temperature: "));
        printToSerial(String(maxTemp));

        serialMutex.lock();
        com.eth.handleEthernetClient();
        serialMutex.unlock();

        yield();
        return true;
    }
};

// class to get sensor data from uC sensors, pack it into json and put it to the endpoints
// currently usable with the WSL python script, soon also with HAS
// TODO: PUT MEAS DATA INTO A QUEUE in order to combat timing issues and network delays
class SensorAndJsonTask final : public frt::Task<SensorAndJsonTask, 2048>
{
public:
    bool run()
    {
        msleep(500);

        // Read the requested endpoint
        String requestedEndpoint = com.eth.getRequestedEndpoint();

        if (requestedEndpoint.length() > 0)
        {
            String jsonBody;

            if (requestedEndpoint == "temperature_sensor_1")
            {
                float temperatureAmb = sens.readSensor(SensorType::AMBIENTTEMPERATURE);
                jsonBody = buildJsonResponse("temperature_sensor_1", temperatureAmb, "Celsius");
            }
            else if (requestedEndpoint == "temperature_sensor_2")
            {
                float temperatureObj = sens.readSensor(SensorType::OBJECTTEMPERATURE);
                jsonBody = buildJsonResponse("temperature_sensor_2", temperatureObj, "Celsius");
            }

            if (jsonBody.length() > 0)
            {
                com.eth.sendJsonResponse(jsonBody);
            }
        }

        yield();

        return true;
    }

private:
    String buildJsonResponse(const String& sensorName, float value, const String& unit)
    {
        String timestamp = getCurrentTimestamp();

        json.clearJson();

        json.createJsonStringConst("sensor_name", sensorName);
        json.createJsonFloat("value", value);
        json.createJsonStringConst("unit", unit);
        json.createJsonString("timestamp", timestamp);

        return json.getJsonString();
    }

    String getCurrentTimestamp()
    {
        time.incrementTime(&currentTime);
        return time.formatTimeString(currentTime);
    }
};


// Task to log time using the time from the compiler
class TimeTask final : public frt::Task<TimeTask, 512>
{
public:
    bool run()
    {
    	msleep(1000);

    	while(1)
    	{
    		msleep(1000);
    		time.incrementTime(&currentTime);
    		printToSerial(F(" "));
    		printToSerial(F("Current Time:"), false);
    		printToSerial(String(currentTime.year), false);
    		printToSerial(F("/"), false);
    		printToSerial(String(currentTime.month), false);
    		printToSerial(F("/"), false);
    		printToSerial(String(currentTime.day), false);
    		printToSerial(F(" "), false);
    		printToSerial(String(currentTime.hour), false);
    		printToSerial(F(":"),false);
    		printToSerial(String(currentTime.minute),false);
    		printToSerial(F(":"), false);
    		printToSerial(String(currentTime.second), false);
    		printToSerial(F(" "));
    		yield();
    	}

    	return true;
    }
};

// Task to talk to and control the VAT uC as a slave, we are able to sen get/set data to handle data flows
class VatTask final : public frt::Task<VatTask, 512>
{
public:
	bool run()
	{
		msleep(1000);
		com.eth.setCompound(ID::ACCESS_MODE, 10, "00");
		com.eth.getCompound(ID::ACTUAL_POSITION, 1);
		yield();
		return true;
	}

private:
};

ReportTask reportTask;
MonitoringTask monitoringTask;
SensorAndJsonTask sensorAndJsonTask;
TimeTask timeTask;
FlyBackTask flybackTask;

void setup()
{
    Serial.begin(9600);

    char date[] = __DATE__; // "MMM DD YYYY"
    char time[] = __TIME__; // "HH:MM:SS"
    char monthStr[4];

    sscanf(date, "%s %d %d", monthStr, &currentTime.day, &currentTime.year);

    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    for (int i = 0; i < 12; i++)
    {
        if (strncmp(monthStr, months[i], 3) == 0)
        {
            currentTime.month = i + 1;
            break;
        }
    }

    sscanf(time, "%d:%d:%d", &currentTime.hour, &currentTime.minute, &currentTime.second);

    Serial.println(F("Starting sensor module..."));
    sens.beginSensor();
    com.i2c.beginI2C(0x78);
    com.spi.beginSPI();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.eth.beginEthernet(mac, ip);

    //For Fylback Test
    pinMode(11, OUTPUT);

    // Clear Timer on Compare Match (CTC) mode with Fast PWM
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // No prescaler
    ICR1 = 16000000 / 1000;  // Default frequency at 1kHz
    OCR1A = ICR1 / 2;         // 50% duty cycle

    if (com.eth.isInitialized())
    {
        Serial.println("Ethernet is running!");
    }

    Serial.println(F("Setup complete."));

    // Start tasks
    reportTask.start(1);
    sensorAndJsonTask.start(2);
    //timeTask.start(3);
    //flybackTask.start(3);
}

void loop()
{
    // No need for loop; tasks handle the work
}


