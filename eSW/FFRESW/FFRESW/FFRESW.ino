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

ComModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;
JsonModuleInternals json;
TimeModuleInternals* _timeMod = TimeModuleInternals::getInstance();
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

// helper method to update time
void updateTime()
{
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 60000; // 60 sec

    if (millis() - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = millis(); // Update timestamp

        String startTime = com.eth.getSpecificEndpoint("time/");
        _timeMod->setTimeFromHas(startTime);
    }
}

/// @brief Task to report system health and status periodically
class ReportTask final : public frt::Task<ReportTask, 200>
{
private:
    uint32_t REPORT_INTERVAL_MS = 5000;
    uint32_t lastReportTime = 0;

public:
    bool run()
    {
        uint32_t currentTime = millis();

        if (currentTime - lastReportTime >= REPORT_INTERVAL_MS)
        {
            lastReportTime = currentTime;

            String status = report.reportStatus(true);
            if (status.length() > 0)
            {
                printToSerial(status);
            }

            bool healthCheck = report.checkSystemHealth(3000, true, false, false, false, false);
            if (!healthCheck)
            {
                printToSerial(F("[CRITICAL] System health check failed!"));
            }
        }

        yield();
        return true;
    }
};

/// @brief Implementation of the FlyBackTask class.
/// @brief Control and use HV module
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

/// @brief Implementation of the MonitoringTask class.
/// @brief Task to monitor queue and system usage
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

        float average = CalcModuleInternals::calculateAverage(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        printToSerial(F("Average Temperature: "));
        printToSerial(String(average));

        float pressure = sens.readSensor(SensorType::PRESSURE);
        printToSerial(F("Current Pressure: "));
        printToSerial(String(pressure));

        float maxTemp = CalcModuleInternals::findMaximum(temperatureReadings, bufferFull ? maxReadings : currentIndex);
        printToSerial(F("Maximum Temperature: "));
        printToSerial(String(maxTemp));

        serialMutex.lock();
        com.eth.handleEthernetClient();
        serialMutex.unlock();

        yield();
        return true;
    }
};

/// @brief Implementation of the SensorActorEndpoint class.
class SensorActorEndpointTask final : public frt::Task<SensorActorEndpointTask, 2048>
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
            else if (requestedEndpoint.startsWith("set_control_mode"))
            {
            	int separatorIndex = requestedEndpoint.indexOf('/');
            	if (separatorIndex != -1)
            	{
            	    String controlModeVal = requestedEndpoint.substring(separatorIndex + 1);
            	    com.eth.setParameter(Compound2::CONTROL_MODE, controlModeVal);
            	    String response = com.eth.getParameter(Compound2::CONTROL_MODE);
            	    jsonBody = buildJsonResponse("control_mode", response.toFloat(), "mode");
            	}
            }
            else if (requestedEndpoint.startsWith("set_target_position"))
            {
            	int separatorIndex = requestedEndpoint.indexOf('/');
            	if (separatorIndex != -1)
            	{
					String positionVal = requestedEndpoint.substring(separatorIndex + 1);
					com.eth.setParameter(Compound2::TARGET_POSITION, positionVal);
					String rawVal = CalcModuleInternals::extractFloat(positionVal, 1);
					jsonBody = buildJsonResponse("set_target_position", rawVal.toFloat(), "position");
            	}
            }
            else if (requestedEndpoint.startsWith("get_actual_position"))
            {
				String response = com.eth.getParameter(Compound2::ACTUAL_POSITION);
				String rawVal = CalcModuleInternals::extractFloat(response, 1);
				jsonBody = buildJsonResponse("get_actual_position", rawVal.toFloat(), "position");

            }
            else if (requestedEndpoint.startsWith("set_target_pressure"))
            {
            	int separatorIndex = requestedEndpoint.indexOf('/');
            	if (separatorIndex != -1)
            	{
					String pressureVal = requestedEndpoint.substring(separatorIndex + 1);
					com.eth.setParameter(Compound2::TARGET_PRESSURE, pressureVal);
					String rawVal = CalcModuleInternals::extractFloat(pressureVal, 1);
					jsonBody = buildJsonResponse("set_target_pressure", rawVal.toFloat(), "pressure");
            	}
            }
            else if (requestedEndpoint.startsWith("get_actual_pressure"))
            {
            	String response = com.eth.getParameter(Compound2::ACTUAL_PRESSURE);
            	String rawVal = CalcModuleInternals::extractFloat(response, 1);
				jsonBody = buildJsonResponse("get_actual_pressure", rawVal.toFloat(), "pressure");
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

        String jsonString = json.getJsonString();
        printToSerial("Generated JSON: " + jsonString);

        return json.getJsonString();
    }

    String getCurrentTimestamp()
    {
    	updateTime();
        return TimeModuleInternals::formatTimeString(_timeMod->getSystemTime());
    }
};

ReportTask reportTask;
MonitoringTask monitoringTask;
SensorActorEndpointTask sensorActorEndpointTask;
FlyBackTask flybackTask;

void setup()
{
    Serial.begin(9600);

    printToSerial("[INFO] Starting up...");

    // init all comModules
    sens.beginSensor();
    com.i2c.beginI2C(0x78);
    com.spi.beginSPI();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.eth.beginEthernet(mac, ip);

    // activate the stackguard
    ReportSystem::initStackGuard();

    // Get latest time from HAS
    String startTime = com.eth.getSpecificEndpoint("time/");
    _timeMod->setTimeFromHas(startTime);

    //For Fylback Test
    pinMode(11, OUTPUT);

    // Clear Timer on Compare Match (CTC) mode with Fast PWM
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // No prescaler
    ICR1 = 16000000 / 1000;  // Default frequency at 1kHz
    OCR1A = ICR1 / 2;         // 50% duty cycle

    printToSerial("[INFO] Setup complete.");

    // Start tasks
    reportTask.start(1);
    sensorActorEndpointTask.start(2);
    //flybackTask.start(3);
}

void loop()
{
    // No need for loop; tasks handle the work
}


