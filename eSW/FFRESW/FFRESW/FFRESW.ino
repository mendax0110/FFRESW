#include <frt.h>
#include <calcModule.h>
#include <sensorModule.h>
#include <comModule.h>
#include <reportSystem.h>
#include <jsonModule.h>
#include <timeModule.h>
#include <serialMenu.h>
#include <flyback.h>
#include <logManager.h>
#include <vacControl.h>
#include <lockGuard.h>

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


// helper method to update time, do not run in a TASK!
void updateTime()
{
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 60000; // 60 sec

    if (millis() - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = millis(); // Update timestamp

        String startTime;
        {
        	//LockGuard lock(ethernetMutex);
        	startTime = com.getEthernet().getSpecificEndpoint("time/");
        }
        _timeMod->setTimeFromHas(startTime);
    }
}

/// @brief Class-Task to report system health and status periodically \class ReportTask
class ReportTask final : public frt::Task<ReportTask, 512>
{
private:
    uint32_t REPORT_INTERVAL_MS = 5000;

public:
    bool run()
    {
    	if (wait(REPORT_INTERVAL_MS))
    	{
    		report.reportStatus(true);

            bool healthCheck = report.checkSystemHealth(3000, true, true, true, false, false);
            if (!healthCheck)
            {
            	SerialMenu::printToSerial(SerialMenu::OutputLevel::CRITICAL, F("System health check failed!"));
            }
    	}
    	else
    	{
            yield();
    	}

        return true;
    }
};
ReportTask reportTask;

/// @brief Implementation of the FlyBackTask class, control and use HV/VAC module \class FlyBackVacControlTask
class FlyBackVacControlTask final : public frt::Task<FlyBackVacControlTask, 512>
{
public:
	bool run()
	{
		if (flyback.isInitialized())
		{
			flyback.run();
			yield(); // DO NOT UNDER ANY CIRCUMSTANCE CHANGE THIS TO SLEEP WE NEED TO YIELD TO THE NEXT TASK
		}

		if (vacControl.isInitialized())
		{
			vacControl.run();

			vacControlModule::MainSwitchStates mainState = vacControl.getMainSwitchState();
			vacControlModule::MainSwitchStates lastmainState = vacControlModule::MainSwitchStates::Main_switch_INVALID;

			if (mainState == vacControlModule::MainSwitchStates::Main_Switch_OFF)
			{
				if (lastAppliedScenario != Scenarios::Scenario_5)
				{
					applyScenario(Scenarios::Scenario_5);
					lastAppliedScenario = Scenarios::Scenario_5;
				}
			}
			else if (mainState == vacControlModule::MainSwitchStates::Main_Switch_MANUAL)
			{
				String response = getEthernetParameterWithLock(Compound2::ACTUAL_PRESSURE);

                float rawValue = CalcModuleInternals::extractFloatFromResponse(response, Type::Pressure);
                vacControl.setExternPressure(rawValue);

				int scenario = vacControl.getScenario();
				if (scenario != lastAppliedScenario)
				{
					applyScenario(scenario);
					lastAppliedScenario = scenario;
				}

				reportTask.post();
				yield();
			}

			msleep(100); //TODO EndpointTask not working properly timout problems
			yield();
		}

		yield();
		return true;
	}

private:

	int lastAppliedScenario = -1;

	void setEthernetParamWithLock(Compound2 param, const String& value)
	{
		LockGuard lock(ethernetMutex);
		com.getEthernet().setParameter(param, value);
	}

	String getEthernetParameterWithLock(Compound2 param)
	{
		LockGuard lock(ethernetMutex);
		return com.getEthernet().getParameter(param);
	}

	void applyScenario(int scenario)
	{
		switch (scenario)
		{
			case Scenarios::Scenario_1:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 1"));
				setEthernetParamWithLock(Compound2::CONTROL_MODE, "3");  //CLOSE
				break;
			}
			case Scenarios::Scenario_2:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 2"));
				setEthernetParamWithLock(Compound2::CONTROL_MODE, "5"); // Pressure Control
				setEthernetParamWithLock(Compound2::TARGET_PRESSURE, "0.1");
				break;
			}
			case Scenarios::Scenario_3:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 3"));
				setEthernetParamWithLock(Compound2::CONTROL_MODE, "5"); // Pressure Control
				setEthernetParamWithLock(Compound2::TARGET_PRESSURE, "0.05");
				break;
			}
			case Scenarios::Scenario_4:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 4"));
				setEthernetParamWithLock(Compound2::CONTROL_MODE, "5"); // Pressure Control
				setEthernetParamWithLock(Compound2::TARGET_PRESSURE, "0.03");
				break;
			}
			case Scenarios::Scenario_5:
			{
				SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, F("Scenario 5"));
				setEthernetParamWithLock(Compound2::CONTROL_MODE, "4");
				break;
			}
			default:
			{
				break;
			}
		}
	}
};
FlyBackVacControlTask flyBackVacControlTask;

/// @brief Implementation of the SensorActorEndpoint class, control Sensors, Actorcs, Task... \class SensorActorEndpointTask
class SensorActorEndpointTask final : public frt::Task<SensorActorEndpointTask, 1024>
{
public:
	bool run()
	{
	    uint32_t now = millis();
	    if (now - lastRequestTime < MIN_REQUEST_INTERVAL)
	    {
	    	yield();
	    	return true;
	    }

	    // Read the requested endpoint
	    String requestedEndpoint;
	    {
	    	LockGuard lock(ethernetMutex);
	    	requestedEndpoint = com.getEthernet().getRequestedEndpoint();
	    }

	    if (requestedEndpoint.length() == 0)
	    {
	    	yield();
	    	return true;
	    }

	    lastRequestTime = now;
	    String jsonBody;
	    bool processed = false;

	    // ReportSystem-Endpoints
        if (requestedEndpoint.startsWith("get_report_"))
        {
            jsonBody = handleReportGet(requestedEndpoint);
            processed = true;
        }

	    // Flyback-Endpoints
        if (requestedEndpoint.startsWith("get_flyback_"))
        {
            jsonBody = handleFlybackGet(requestedEndpoint);
            processed = true;
        }
        else if (requestedEndpoint.startsWith("set_flyback_"))
        {
            jsonBody = handleFlybackSet(requestedEndpoint);
            processed = true;
        }

        // VacControl-Endpoints
        if(requestedEndpoint.startsWith("get_vacControl_"))
        {
        	jsonBody = handleVacControlGet(requestedEndpoint);
        	processed = true;
        }

        // Temperature-Sensor-Endpoints
        if (requestedEndpoint.startsWith("get_temperature_"))
        {
            handleTemperatureSensors(requestedEndpoint, jsonBody);
            processed = true;
        }

        // Vacuumpump-Endpoints
        if (requestedEndpoint.startsWith("set_pump/"))
        {
        	jsonBody = handleVacuumPumpSet(requestedEndpoint);
        	processed = true;
        }
        else if(requestedEndpoint.startsWith("get_pump_"))
		{
        	jsonBody = handleVacuumPumpGet(requestedEndpoint);
        	processed = true;
		}

	    // VAT-Endpoints
	    if (requestedEndpoint.startsWith("set_"))
	    {
	        processSetRequest(requestedEndpoint, jsonBody);
	        processed = true;
	    }
	    else if (requestedEndpoint.startsWith("get_"))
	    {
	        processGetRequest(requestedEndpoint, jsonBody);
	        processed = true;
	    }

	    // Reboot-Endpoint
	    if (requestedEndpoint.startsWith("REBOOT"))
	    {
	    	handleReboot(requestedEndpoint, jsonBody);
	    	processed = true;
	    }

	    // Last guard, to check if the jsonbody length
	    if (processed && jsonBody.length() > 0)
	    {
	    	{
	    		LockGuard lock(ethernetMutex);
	    		com.getEthernet().sendJsonResponse(jsonBody);
	    	}
	        reportTask.post();
	    }

	    yield();
	    return true;
	}

private:

	uint32_t lastRequestTime = 0;
	const uint32_t MIN_REQUEST_INTERVAL = 100;

    String buildJsonResponse(const String& sensorName, float value, const String& unit)
    {
        String timestamp = getCurrentTimestamp();

        json.clearJson();

        json.createJson("sensor_name", sensorName);
        json.createJson("value", value);
        json.createJson("unit", unit);
        json.createJson("timestamp", timestamp);

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

        if (command == "control_mode")
        {
        	String response;
        	{
        		LockGuard lock(ethernetMutex);
                com.getEthernet().setParameter(param, valueStr);
                response = com.getEthernet().getParameter(param);
        	}
            jsonBody = buildJsonResponse("control_mode", response.toFloat(), "mode");
        }
        else if (command == "target_position")
        {
        	{
        		LockGuard lock(ethernetMutex);
        		com.getEthernet().setParameter(param, valueStr);
        	}
            float rawVal = CalcModuleInternals::extractFloat(valueStr, 1);
            jsonBody = buildJsonResponse("target_position", rawVal, "position");
        }
        else if (command == "target_pressure")
        {
        	{
        		LockGuard lock(ethernetMutex);
        		com.getEthernet().setParameter(param, valueStr);
        	}
            float rawVal = CalcModuleInternals::extractFloat(valueStr, 1);
            jsonBody = buildJsonResponse("target_pressure", rawVal, "pressure");
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

        String response;
        {
        	LockGuard lock(ethernetMutex);
        	response = com.getEthernet().getParameter(param);
        }

        SerialMenu::printToSerial(SerialMenu::OutputLevel::DEBUG, "Response from VAT: " + response);

        float rawVal = CalcModuleInternals::extractFloat(response, 0);
        jsonBody = buildJsonResponse(requestedEndpoint, rawVal, command.endsWith("position") ? "position" : "mbar");
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
        if (command == "main_switch") return buildJsonResponse("main_switch", static_cast<int>(flyback.getMainSwitchState()), "state");
        if (command == "psu_state") return buildJsonResponse("psu_state", static_cast<int>(flyback.getHVState()), "state");

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
        else if (command == "psu_state")
        {
        	int psuState = valueStr.toInt();
        	flyback.setExternPSU(psuState);
        	return buildJsonResponse("psu_state", psuState, "state");
        }
        return "";
    }

    String handleVacControlGet(const String& cmd)
    {
    	String command = cmd.substring(15);

    	if(command == "targetpressure")
    	{
    		float target = vacControl.getCurrentTargetPressure();
    		return buildJsonResponse("target_pressure", target, "bar");
    	}
    	return "";
    }

    String handleVacuumPumpGet(const String& cmd)
    {
    	String command = cmd.substring(9);
        if (command == "state") return buildJsonResponse("pump_state", static_cast<int>(vacControl.getPumpState()), "state");
        return "";
    }

    String handleVacuumPumpSet(const String& cmd)
    {
        String valueStr = cmd.substring(9);
        int pumpState = valueStr.toInt();
        vacControl.setExternPump(pumpState);
        return buildJsonResponse("pump", pumpState, "state");
    }

    void handleTemperatureSensors(const String& requestedEndpoint, String& jsonBody)
    {
    	String command = requestedEndpoint.substring(16);

        if (command == "MCP9601C_Indoor")
        {
        	float value = sens.readSensor(SensorType::MCP9601_Celsius_Indoor);
        	jsonBody = buildJsonResponse(requestedEndpoint, value, "C");
        }
        else if (command == "MCP9601C_Outdoor")
        {
        	float value = sens.readSensor(SensorType::MCP9601_Celsius_Outdoor);
        	jsonBody = buildJsonResponse(requestedEndpoint, value, "C");
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
        if (strcmp(command, "tempSensOk") == 0)
        {
        	bool tempSens = report.isTemperatureSensorOK();
        	return buildJsonResponse("tempSensOk", tempSens, "bool");
        }
        if (strcmp(command, "memoryOk") == 0)
        {
        	bool memory = report.isMemoryOK();
        	return buildJsonResponse("memoryOk", memory, "bool");
        }
        if (strcmp(command, "ramOk") == 0)
        {
        	bool ram = report.isRamOK();
        	return buildJsonResponse("ramOk", ram, "bool");
        }
        if (strcmp(command, "stackSafeOk") == 0)
        {
        	bool stackSafe = report.isStackSafe();
        	return buildJsonResponse("stackSafeOk", stackSafe, "bool");
        }

        return "";
    }

    void handleReboot(const String& requestedEndpoint, const String& jsonBody)
    {
    	jsonBody = buildJsonResponse(requestedEndpoint, 1, "bool");
    	{
    		LockGuard lock(ethernetMutex);
    		com.getEthernet().sendJsonResponse(jsonBody);
    	}
    	msleep(1000);
    	hardRestart();
    }
};
SensorActorEndpointTask sensorActorEndpointTask;

/// @brief Implementation of theStackMonitorTask Class, Handles the Stacks of all running tasks. \class StackMonitorTask
class StackMonitorTask final : public frt::Task<StackMonitorTask, 256>
{
public:
    static const unsigned int REPORT_TASK_STACK_LIMIT = 512;
    static const unsigned int MONITORING_TASK_STACK_LIMIT = 128;
    static const unsigned int SENSOR_ACTOR_TASK_STACK_LIMIT = 1024;
    static const unsigned int FLYBACK_VAC_TASK_STACK_LIMIT = 512;

    static const float THRESHOLD = 0.8f;
    static const float ERR_THRESHOLD = 0.9f;

    bool run()
    {
        checkAndReport("reportTask", reportTask.getUsedStackSize(), REPORT_TASK_STACK_LIMIT);
        checkAndReport("sensorActorEndpointTask", sensorActorEndpointTask.getUsedStackSize(), SENSOR_ACTOR_TASK_STACK_LIMIT);
        checkAndReport("flyBackVacControlTask", flyBackVacControlTask.getUsedStackSize(), FLYBACK_VAC_TASK_STACK_LIMIT);

        msleep(1000);
        reportTask.post();
        return true;
    }

private:
    void checkAndReport(const String& taskName, unsigned int used, unsigned int limit)
    {
        if (used >= static_cast<unsigned int>(limit * THRESHOLD))
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::WARNING, "Stack usage high for " + taskName + ": " + String(used) + " / " + String(limit));
        }
        else if (used >= static_cast<unsigned int>(limit * ERR_THRESHOLD))
        {
        	gracefulRestart();
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
    com.getSerial().endSerial();
    com.getI2C().endI2C();
    com.getSPI().endSPI();
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
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to init SD-Card..."), true);
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

    //neutronDetector.initialize();

    byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
    IPAddress ip(192, 168, 1, 3);
    com.getEthernet().beginEthernet(mac, ip);

    com.getI2C().beginI2CGlobal();
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
    sensorActorEndpointTask.start(2); // was 2
    flyBackVacControlTask.start(3); // was 3
}

void loop()
{
    // No need for loop; tasks handle the work
}


