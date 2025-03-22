#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <comModule.h>
#include <reportSystem.h>
#include <jsonModule.h>
#include <timeModule.h>
#include <serialMenu.h>
#include <flyback.h>

using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;
using namespace comModule;
using namespace jsonModule;
using namespace timeModule;
using namespace flybackModule;

ComModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;
JsonModuleInternals json;
TimeModuleInternals* _timeMod = TimeModuleInternals::getInstance();
DateTimeStruct currentTime;
Flyback flyback;

// Buffers for sensor data
uint8_t i2cBuffer[10];
uint8_t spiBuffer[10];
char ethernetBuffer[256];

frt::Queue<float, 1> temperatureQueue;
frt::Mutex temperatureQueueMutex;


// helper method to update time
void updateTime()
{
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 60000; // 60 sec

    if (millis() - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = millis(); // Update timestamp

        String startTime = com.getEthernet().getSpecificEndpoint("time/");
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
            	SerialMenu::printToSerial(status);
            }

            bool healthCheck = report.checkSystemHealth(3000, true, true, true, false, false);
            if (!healthCheck)
            {
            	SerialMenu::printToSerial(F("[CRITICAL] System health check failed!"));
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

public:
	// Hauptlogik des Tasks
	bool run()
	{
		if(flyback.isInitialized())
		{
			flyback.run();

			//Verzögerung, um den Task nicht zu überlasten
			msleep(1000);
		}
		return true;
	}
};

/// @brief Implementation of the MonitoringTask class.
/// @brief Task to monitor queue and system usage
class MonitoringTask final : public frt::Task<MonitoringTask>
{
private:

public:
    bool run()
    {
        msleep(1000);
        yield();
        //TODO Add LCD Diplay Stuff here!!

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
	    String requestedEndpoint = com.getEthernet().getRequestedEndpoint();
	    if (requestedEndpoint.length() == 0) return true;

	    String jsonBody;

	    // Direct lookup table for simple temperature sensors
	    static const std::map<String, SensorType> sensorMap = {
	        {"temperature_sensor_1", SensorType::AMBIENTTEMPERATURE},
	        {"temperature_sensor_2", SensorType::OBJECTTEMPERATURE}
	    };

	    if (requestedEndpoint.startsWith("get_flyback_"))
	    {
	        String command = requestedEndpoint.substring(12);

	        if (command == "voltage")
	        {
	        	Measurement result = flyback.measure();
	        	SerialMenu::printToSerial("Voltage: " + String(result.voltage) + " V");
	            jsonBody = buildJsonResponse("voltage", result.voltage, "V");
	        }
	        else if (command == "current")
	        {
	        	Measurement result = flyback.measure();
	        	SerialMenu::printToSerial("Current: " + String(result.current) + " uA");
	            jsonBody = buildJsonResponse("current", result.current, "uA");
	        }
	        else if (command == "power")
	        {
	            Measurement result = flyback.measure();
	            SerialMenu::printToSerial("Power: " + String(result.power) + " uW");
	            jsonBody = buildJsonResponse("power", result.power, "uW");
	        }
	        else if (command == "digital_value")
	        {
	            Measurement result = flyback.measure();
	            SerialMenu::printToSerial("DigitalValue: " + String(result.digitalValue) + " ");
	            jsonBody = buildJsonResponse("digital_value", result.digitalValue, "");
	        }
	        else if (command == "frequency")
	        {
	        	Measurement result = flyback.measure();
	        	SerialMenu::printToSerial("Frequency: " + String(result.frequency) + "Hz");
	            jsonBody = buildJsonResponse("frequency", result.frequency, "Hz");
	        }
	        else if (command == "switch_state")
	        {
	        	int swStat = static_cast<int>(flyback.getSwitchState());
	        	SerialMenu::printToSerial("Current SwitchState is:" + swStat);
	        	jsonBody = buildJsonResponse("switch_state", swStat, "state");
	        }
	    }
	    else if (requestedEndpoint.startsWith("set_flyback_"))
	    {
	        String command = requestedEndpoint.substring(12);
	        int separatorIndex = command.indexOf('/');

	        if (separatorIndex == -1)
	        {
	            return true;
	        }

	        String valueStr = command.substring(separatorIndex + 1);
	        command = command.substring(0, separatorIndex);

	        if (command == "frequency")
	        {
	            int frequency = valueStr.toInt();
	            flyback.setExternFrequency(frequency);
	            jsonBody = buildJsonResponse("flyback_frequency", frequency, "Hz");
	        }
	    }


	    auto sensorIt = sensorMap.find(requestedEndpoint);
	    if (sensorIt != sensorMap.end())
	    {
	        float value = sens.readSensor(sensorIt->second);
	        jsonBody = buildJsonResponse(requestedEndpoint, value, "Celsius");
	    }
	    else if (requestedEndpoint.startsWith("set_"))
	    {
	        processSetRequest(requestedEndpoint, jsonBody);
	    }
	    else if (requestedEndpoint.startsWith("get_"))
	    {
	        processGetRequest(requestedEndpoint, jsonBody);
	    }

	    if (jsonBody.length() > 0)
	    {
	        com.getEthernet().sendJsonResponse(jsonBody);
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
        SerialMenu::printToSerial("Generated JSON: " + jsonString);

        return json.getJsonString();
    }

    String getCurrentTimestamp()
    {
    	updateTime();
        return TimeModuleInternals::formatTimeString(_timeMod->getSystemTime());
    }

    void processSetRequest(const String& requestedEndpoint, String& jsonBody)
    {
        int separatorIndex = requestedEndpoint.indexOf('/');
        if (separatorIndex == -1) return; // Invalid request format

        String valueStr = requestedEndpoint.substring(separatorIndex + 1);
        String command = requestedEndpoint.substring(4, separatorIndex); // Extracts parameter name

        static const std::map<String, Compound2> setParams = {
            {"control_mode", Compound2::CONTROL_MODE},
            {"target_position", Compound2::TARGET_POSITION},
            {"target_pressure", Compound2::TARGET_PRESSURE}
        };

        auto it = setParams.find(command);
        if (it != setParams.end())
        {
            com.getEthernet().setParameter(it->second, valueStr);

            if (command == "control_mode")  // Read back for verification
            {
                String response = com.getEthernet().getParameter(it->second);
                jsonBody = buildJsonResponse("control_mode", response.toFloat(), "mode");
                SerialMenu::printToSerial(jsonBody);
            }
            else
            {
                String rawVal = CalcModuleInternals::extractFloat(valueStr, 1);
                jsonBody = buildJsonResponse(requestedEndpoint, rawVal.toFloat(), command.endsWith("position") ? "position" : "pressure");
                SerialMenu::printToSerial(jsonBody);
            }
        }
    }

    void processGetRequest(const String& requestedEndpoint, String& jsonBody)
    {
        String command = requestedEndpoint.substring(4); // Extracts parameter name

        static const std::map<String, Compound2> getParams = {
            {"actual_position", Compound2::ACTUAL_POSITION},
            {"actual_pressure", Compound2::ACTUAL_PRESSURE}
        };

        auto it = getParams.find(command);
        if (it != getParams.end())
        {
            String response = com.getEthernet().getParameter(it->second);
            String rawVal = CalcModuleInternals::extractFloat(response, 1);
            jsonBody = buildJsonResponse(requestedEndpoint, rawVal.toFloat(), command.endsWith("position") ? "position" : "pressure");
            SerialMenu::printToSerial(jsonBody);
        }
    }
};

ReportTask reportTask;
MonitoringTask monitoringTask;
SensorActorEndpointTask sensorActorEndpointTask;
FlyBackTask flybackTask;

void setup()
{
    //Serial.begin(9600);
    com.getSerial().beginSerial(9600);

    SerialMenu::printToSerial("[INFO] Starting up...");

    // init all comModules
    sens.beginSensor();
    flyback.initialize();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.getEthernet().beginEthernet(mac, ip);

    com.getI2C().beginI2C(0x76);
    com.getSPI().beginSPI();

    // activate the stackguard
    ReportSystem::initStackGuard();

    // Get latest time from HAS
    String startTime = com.getEthernet().getSpecificEndpoint("time/");
    _timeMod->setTimeFromHas(startTime);


    SerialMenu::printToSerial("[INFO] Setup complete.");

    // Start tasks
    reportTask.start(1);
    sensorActorEndpointTask.start(2);
    flybackTask.start(3);
}

void loop()
{
    // No need for loop; tasks handle the work
}

// TODO CHECK IF REDESIGN OF COMMODULE IS WORKING OR NOT!!!!


