#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <comModule.h>
#include <reportSystem.h>
#include <jsonModule.h>
#include <timeModule.h>
#include <serialMenu.h>
#include <flyback.h>
#include <logManager.h>	// TODO CHECK THIS NEWLY ADDED KEEP MEMORY IN MIND!
#include <vacControl.h>

using namespace calcModule;
using namespace sensorModule;
using namespace reportSystem;
using namespace comModule;
using namespace jsonModule;
using namespace timeModule;
using namespace flybackModule;
using namespace vacControlModule;

ComModuleInternals com;
SensorModuleInternals sens;
ReportSystem report;
JsonModuleInternals json;
TimeModuleInternals* _timeMod = TimeModuleInternals::getInstance();
DateTimeStruct currentTime;
Flyback flyback;
VacControl vacControl;

// Buffers for sensor data
#ifdef USE_I2C
uint8_t i2cBuffer[10];
#endif

#ifdef USE_SPI
uint8_t spiBuffer[10];
#endif

#ifdef USE_ETHERNET
char ethernetBuffer[256];
#endif


frt::Queue<float, 1> temperatureQueue;
frt::Queue<String, 5> endpointQueue;

frt::Mutex temperatureQueueMutex;
frt::Mutex ethernetMutex;


// helper method to update time -> do not run in a TASK!!!
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

/// @brief Class-Task to report system health and status periodically \class ReportTask
class ReportTask final : public frt::Task<ReportTask, 512> // TODO Check usage on stack was previously 256
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

            report.reportStatus(true);

            bool healthCheck = report.checkSystemHealth(3000, true, true, true, false, false);
            if (!healthCheck)
            {
            	SerialMenu::printToSerial(SerialMenu::OutputLevel::CRITICAL, F("System health check failed!"));
            }
        }
        yield();
        return true;
    }
};

/// @brief Implementation of the FlyBackTask class, control and use HV/VAC module \class FlyBackVacControlTask
class FlyBackVacControlTask final : public frt::Task<FlyBackVacControlTask, 512> // TODO Check stack size, was previoulsy std 256
{
public:
	bool run()
	{
		if (flyback.isInitialized())
		{
			flyback.run();
			msleep(1000);
		}


		if (vacControl.isInitialized())
		{
			vacControl.run();

			vacControlModule::SwitchStates swState = vacControl.getSwitchState();
			if (swState == vacControlModule::SwitchStates::Main_Switch_MANUAL)
			{
                String response;

                {
                    ethernetMutex.lock();
                    response = com.getEthernet().getParameter(Compound2::ACTUAL_PRESSURE);
                    ethernetMutex.unlock();
                }

                float rawValue = CalcModuleInternals::extractFloatFromResponse(response, Type::Pressure);
                vacControl.setExternPressure(rawValue);

				int scenario = vacControl.getScenario();
				applyScenario(scenario);

			}

			msleep(1000);
		}

		yield();
		return true;
	}

private:
	// TODO: CHECK THIS WITH FRAMDOM01 AND SEE IF IT WORKS!!
	void applyScenario(int scenario)
	{
		switch (scenario)
		{
			case Scenarios::Scenario_1:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 1"));
				com.getEthernet().setParameter(Compound2::CONTROL_MODE, "3"); //CLOSE
				break;
			}
			case Scenarios::Scenario_2:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 2"));
				com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5"); // Pressure Control
				com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "1");
				break;
			}
			case Scenarios::Scenario_3:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 3"));
				com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5"); // Pressure Control
				com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "0.8");
				break;
			}
			case Scenarios::Scenario_4:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 4"));
				com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5"); // Pressure Control
				com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "0.6");
				break;
			}
			case Scenarios::Scenario_5:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 5"));
				com.getEthernet().setParameter(Compound2::CONTROL_MODE, "4"); // OPEN
				break;
			}
			default:
			{
				break;
			}
		}
	}
};

/// @brief Implementation of the SensorActorEndpoint class, control Sensors, Actorcs, Task... \class SensorActorEndpointTask
class SensorActorEndpointTask final : public frt::Task<SensorActorEndpointTask, 1024>  // TODO: was 2048 really needed?? -> evaluate!
{
public:
	bool run()
	{
	    msleep(500);

	    // Read the requested endpoint
	    String requestedEndpoint = com.getEthernet().getRequestedEndpoint();
	    if (requestedEndpoint.length() == 0) return true;

	    String jsonBody;

	    // ReportSystem-Endpoints
        if (requestedEndpoint.startsWith("get_report_"))
        {
            jsonBody = handleReportGet(requestedEndpoint);
        }

	    // Flyback-Endpoints
        if (requestedEndpoint.startsWith("get_flyback_"))
        {
            jsonBody = handleFlybackGet(requestedEndpoint);
        }
        else if (requestedEndpoint.startsWith("set_flyback_"))
        {
            jsonBody = handleFlybackSet(requestedEndpoint);
        }

        // VacControl-Endpoints
        if (requestedEndpoint.startsWith("get_vacControl_"))
        {
            jsonBody = handleFlybackGet(requestedEndpoint);
        }
        else if (requestedEndpoint.startsWith("set_vacControl_"))
        {
            jsonBody = handleFlybackSet(requestedEndpoint);
        }

        // Scenario-Endpoints
        if (requestedEndpoint.startsWith("set_scenario/"))
        {
            jsonBody = handleScenarioSet(requestedEndpoint);
        }
        else if (requestedEndpoint.startsWith("get_scenario/"))
        {
        	jsonBody = handleScenarioGet(requestedEndpoint);
        }

        // Temperature-Sensor-Endpoints
        if (requestedEndpoint.startsWith("get_temperature_"))
        {
            handleTemperatureSensors(requestedEndpoint, jsonBody);
        }

	    // VAT-Endpoints
	    if (requestedEndpoint.startsWith("set_"))
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
        SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, "Generated JSON: " + jsonString);

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
        if (separatorIndex == -1) return;

        String valueStr = requestedEndpoint.substring(separatorIndex + 1);
        String command = requestedEndpoint.substring(4, separatorIndex);

        Compound2 param;

        if (command == "control_mode")
        {
            param = Compound2::CONTROL_MODE;
        }
        else if (command == "target_position")
        {
            param = Compound2::TARGET_POSITION;
        }
        else if (command == "target_pressure")
        {
            param = Compound2::TARGET_PRESSURE;
        }
        else
        {
            return;
        }

        com.getEthernet().setParameter(param, valueStr);

        if (command == "control_mode")
        {
            String response = com.getEthernet().getParameter(param);
            jsonBody = buildJsonResponse("control_mode", response.toFloat(), "mode");
            SerialMenu::printToSerial(jsonBody);
        }
        else
        {
            float rawVal = CalcModuleInternals::extractFloat(valueStr, 1);
            jsonBody = buildJsonResponse(requestedEndpoint, rawVal, command.endsWith("position") ? "position" : "pressure");
            SerialMenu::printToSerial(jsonBody);
        }
    }

    void processGetRequest(const String& requestedEndpoint, String& jsonBody)
    {
        String command = requestedEndpoint.substring(4);
        Compound2 param;

        if (command == "actual_position")
        {
            param = Compound2::ACTUAL_POSITION;
        }
        else if (command == "actual_pressure")
        {
            param = Compound2::ACTUAL_PRESSURE;
        }
        else
        {
            return;
        }

        String response = com.getEthernet().getParameter(param);
        SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, "Response from VAT: " + response);

        float rawVal = CalcModuleInternals::extractFloat(response, 0);
        jsonBody = buildJsonResponse(requestedEndpoint, rawVal, command.endsWith("position") ? "position" : "pressure");
        SerialMenu::printToSerial(jsonBody);
    }

    String handleFlybackGet(const String& cmd)
    {
        String command = cmd.substring(12);
        Measurement result = flyback.measure();

        if (command == "voltage") return buildJsonResponse("voltage", result.voltage, "V");
        if (command == "current") return buildJsonResponse("current", result.current, "uA");
        if (command == "power") return buildJsonResponse("power", result.power, "uW");
        if (command == "digital_freq_value") return buildJsonResponse("digital_freq_value", result.digitalFreqValue, "");
        if (command == "frequency") return buildJsonResponse("frequency", result.frequency, "Hz");
        if (command == "digital_duty_value") return buildJsonResponse("digital_duty_value", result.digitalDutyValue, "");
        if (command == "dutyCycle") return buildJsonResponse("dutyCycle", result.dutyCycle, "%");
        if (command == "switch_state") return buildJsonResponse("switch_state", static_cast<int>(flyback.getSwitchState()), "state");

        return "";
    }

    String handleFlybackSet(const String& cmd)
    {
        String command = cmd.substring(12);
        int separatorIndex = command.indexOf('/');
        if (separatorIndex == -1) return "";

        String valueStr = command.substring(separatorIndex + 1);
        command = command.substring(0, separatorIndex);

        if (command == "frequency")
        {
            uint32_t frequency = valueStr.toInt();
            flyback.setExternFrequency(frequency);
            return buildJsonResponse("flyback_frequency", frequency, "Hz");
        }
        else if (command == "dutyCycle")
        {
            int dutyCycle = valueStr.toInt();
            flyback.setExternDutyCycle(dutyCycle);
            return buildJsonResponse("dutyCycle", dutyCycle, "%");
        }
        return "";
    }

    String handleScenarioSet(const String& cmd)
    {
        String scenarioStr = cmd.substring(13);
        int scenarioId = scenarioStr.toInt();
        int scenarioEnum = 0;
        String label;

        switch (scenarioId)
        {
            case 1:
                scenarioEnum = static_cast<int>(Scenarios::Scenario_1);
                vacControl.setExternScenario(scenarioEnum);
                com.getEthernet().setParameter(Compound2::CONTROL_MODE, "3"); // CLOSE
                label = "Scenario 1";
                break;
            case 2:
                scenarioEnum = static_cast<int>(Scenarios::Scenario_2);
                vacControl.setExternScenario(scenarioEnum);
                com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5"); // Pressure Control
                com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "1");
                label = "Scenario 2";
                break;
            case 3:
                scenarioEnum = static_cast<int>(Scenarios::Scenario_3);
                vacControl.setExternScenario(scenarioEnum);
                com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5");
                com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "0.8");
                label = "Scenario 3";
                break;
            case 4:
                scenarioEnum = static_cast<int>(Scenarios::Scenario_4);
                vacControl.setExternScenario(scenarioEnum);
                com.getEthernet().setParameter(Compound2::CONTROL_MODE, "5");
                com.getEthernet().setParameter(Compound2::TARGET_PRESSURE, "0.6");
                label = "Scenario 4";
                break;
            case 5:
                scenarioEnum = static_cast<int>(Scenarios::Scenario_5);
                vacControl.setExternScenario(scenarioEnum);
                com.getEthernet().setParameter(Compound2::CONTROL_MODE, "4"); // OPEN
                label = "Scenario 5";
                break;
            default:
                SerialMenu::printToSerial("Invalid scenario number: " + scenarioStr);
                return "";
        }

        return buildJsonResponse(label, scenarioEnum, "int");
    }


    String handleScenarioGet(const String& cmd)
    {
    	String command = cmd.substring(15);
    	// TODO Implement the Getters and create new Endpoints for HAS to know where we are.
    	return "";
    }

    void handleTemperatureSensors(const String& requestedEndpoint, String& jsonBody)
    {
    	String command = requestedEndpoint.substring(16);

        if (command == "MCP9601C")
        {
        	float value = sens.readSensor(SensorType::MCP9601_Celsius);
        	jsonBody = buildJsonResponse(requestedEndpoint, value, "°C");
        }
        else if (command == "MCP9601F")
        {
        	float value = sens.readSensor(SensorType::MCP9601_Fahrenheit);
        	jsonBody = buildJsonResponse(requestedEndpoint, value, "F");
        }
        else if (command == "MCP9601K")
        {
        	float value = sens.readSensor(SensorType::MCP9601_Fahrenheit);
        	jsonBody = buildJsonResponse(requestedEndpoint, value, "K");
        }
    }

    String handleReportGet(const String& cmd)
    {
        const char* command = cmd.c_str() + 11;

        if (strcmp(command, "stackOverflow") == 0)
        {
            bool stackOverflow = report.detectStackOverflow();
            return buildJsonResponse("stackOverflow", stackOverflow, "bool");
        }
        if (strcmp(command, "ethernet") == 0)
        {
            bool ethernet = report.checkSystemHealth(3000, true, false, false, false, false);
            return buildJsonResponse("ethernet", ethernet, "bool");
        }
        if (strcmp(command, "spi") == 0)
        {
            bool spi = report.checkSystemHealth(3000, false, true, false, false, false);
            return buildJsonResponse("spi", spi, "bool");
        }
        if (strcmp(command, "i2c") == 0)
        {
            bool i2c = report.checkSystemHealth(3000, false, false, true, false, false);
            return buildJsonResponse("i2c", i2c, "bool");
        }
        if (strcmp(command, "temp") == 0)
        {
            bool temp = report.checkSystemHealth(3000, false, false, false, true, false);
            return buildJsonResponse("temp", temp, "bool");
        }
        if (strcmp(command, "press") == 0)
        {
            bool press = report.checkSystemHealth(3000, false, false, false, false, true);
            return buildJsonResponse("press", press, "bool");
        }

        return "";
    }

};

ReportTask reportTask;
SensorActorEndpointTask sensorActorEndpointTask;
FlyBackVacControlTask flyBackVacControlTask;

/// @brief Implementation of theStackMonitorTask Class, Handles the Stacks of all running tasks. \class StackMonitorTask
class StackMonitorTask final : public frt::Task<StackMonitorTask, 256>
{
public:
    static const unsigned int REPORT_TASK_STACK_LIMIT = 512;
    static const unsigned int MONITORING_TASK_STACK_LIMIT = 128;
    static const unsigned int SENSOR_ACTOR_TASK_STACK_LIMIT = 1024;
    static const unsigned int FLYBACK_VAC_TASK_STACK_LIMIT = 512;

    static const float THRESHOLD = 0.8f; // 80% threshold was previously 0.8f

    bool run()
    {
        checkAndReport("reportTask", reportTask.getUsedStackSize(), REPORT_TASK_STACK_LIMIT);
        checkAndReport("sensorActorEndpointTask", sensorActorEndpointTask.getUsedStackSize(), SENSOR_ACTOR_TASK_STACK_LIMIT);
        checkAndReport("flyBackVacControlTask", flyBackVacControlTask.getUsedStackSize(), FLYBACK_VAC_TASK_STACK_LIMIT);

        msleep(1000);
        return true;
    }

private:
    void checkAndReport(const String& taskName, unsigned int used, unsigned int limit)
    {
        if (used >= static_cast<unsigned int>(limit * THRESHOLD))
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::WARNING, "Stack usage high for " + taskName + ": " + String(used) + " / " + String(limit));
        }
    }
};


StackMonitorTask stackMonitorTask;


void gracefulRestart()
{
    if (flyBackVacControlTask.isRunning()) flyBackVacControlTask.stop();
    if (sensorActorEndpointTask.isRunning()) sensorActorEndpointTask.stop();
    if (reportTask.isRunning()) reportTask.stop();
    if (stackMonitorTask.isRunning()) stackMonitorTask.stop();

    LogManager::getInstance()->flushLogs();

    vacControl.deinitialize();
    flyback.deinitialize();

    flyback.initialize();
    vacControl.initialize();

    stackMonitorTask.start(1);
    reportTask.start(1);
    sensorActorEndpointTask.start(2);
    flyBackVacControlTask.start(3);
}

void hardRestart()
{
	wdt_enable(WDTO_500MS);
	while (true) {}
}


void setup()
{
    com.getSerial().beginSerial(9600);

    // TODO check functionality with different SD-CARD, issues with SPI?
    LogManager::getInstance()->initSDCard(4);
    if (!LogManager::getInstance()->isSDCardInitialized())
    {
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init SD-Card..."));
    }
    else
    {
    	LogManager::getInstance()->setLogFileName("log.txt");
    }

    SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Starting up..."));

    // init all comModules
    sens.initialize();
    flyback.initialize();
    vacControl.initialize();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.getEthernet().beginEthernet(mac, ip);

    //com.getI2C().beginI2C(0x76);
    com.getI2C().beginI2C(0x67);
    com.getSPI().beginSPI();

    // activate the stackguard
    ReportSystem::initStackGuard();

    // Get latest time from HAS
    String startTime = com.getEthernet().getSpecificEndpoint("time/");
    _timeMod->setTimeFromHas(startTime);


    SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Setup complete."));

    // Start tasks
    stackMonitorTask.start(1);
    reportTask.start(1);
    sensorActorEndpointTask.start(2);
    flyBackVacControlTask.start(3);
}

void loop()
{
    // No need for loop; tasks handle the work
}


