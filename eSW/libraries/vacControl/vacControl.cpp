#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <vacControl.h>
#include <serialMenu.h>

using namespace vacControlModule;

int VacControl::lastState = -1;
int VacControl::lastPumpState = -1;

VacControl::VacControl()
{
	_vacControlInitialized = false;
}

VacControl::~VacControl()
{

}

void VacControl::initialize()
{
	// Main Switch Inputs
	pinMode(Main_Switch_OFF, INPUT_PULLUP);
	pinMode(Main_Switch_MANUAL, INPUT_PULLUP);
	pinMode(Main_Switch_REMOTE, INPUT_PULLUP);

	// Pump and Control Elements
	pinMode(Switch_Pump_ON, INPUT_PULLUP);			//TASTER PUMP ON
	pinMode(targetPressure, INPUT);					//POTI FOR Pressure Regulation
	pinMode(Pump_Relay, OUTPUT);					//Pumpen Relais
	pinMode(Pump_Status_LED, OUTPUT);				//Status LED für Pumpe
	pinMode(targetVacuumLED, OUTPUT);				//Status LED für Vakuum

	_vacControlInitialized = true;
	SerialMenu::printToSerial(F("[INFO] VacControl module initialized."));
}

// TODO: TALK TO FRADOM01 about this, whats the best way?!
void VacControl::deinitialize()
{
    // Reset for Main Switch Pins
    pinMode(Main_Switch_OFF, INPUT);
    pinMode(Main_Switch_MANUAL, INPUT);
    pinMode(Main_Switch_REMOTE, INPUT);

    // Reset Pump Pins
    pinMode(Switch_Pump_ON, INPUT);
    pinMode(targetPressure, INPUT);
    pinMode(Pump_Relay, INPUT);
    pinMode(Pump_Status_LED, INPUT);
    pinMode(targetVacuumLED, INPUT);

    // Turn everything off
    digitalWrite(Pump_Relay, LOW);
    digitalWrite(Pump_Status_LED, LOW);
    digitalWrite(targetVacuumLED, LOW);

    _vacControlInitialized = false;
    SerialMenu::printToSerial(F("[INFO] VacControl module deinitialized."));
}

bool VacControl::isInitialized() const
{
	return _vacControlInitialized;
}

MainSwitchStates VacControl::getMainSwitchState()
{
	int off = digitalRead(Main_Switch_OFF);
	int manual = digitalRead(Main_Switch_MANUAL);
	int remote = digitalRead(Main_Switch_REMOTE);

	if(off == LOW) return MainSwitchStates::Main_Switch_OFF;
	if(manual == LOW) return MainSwitchStates::Main_Switch_MANUAL;
	if(remote == LOW) return MainSwitchStates::Main_Switch_REMOTE;

    return MainSwitchStates::Main_switch_INVALID;
}

PumpState VacControl::getPumpState()
{
	int pumpRelay = digitalRead(Pump_Relay);

	if(pumpRelay == HIGH) return PumpState::pump_ON;
	return PumpState::pump_OFF;
}

Pressure VacControl::measure()
{
	int manual = digitalRead(Main_Switch_MANUAL);
	int remote = digitalRead(Main_Switch_REMOTE);

	if(manual == LOW || remote == LOW)
	{
		meas.pressure = getExternPressure();  // Wert auslesen und speichern
	}
	return meas;
}

Scenarios VacControl::getScenario()
{
	MainSwitchStates mode = getMainSwitchState();
	int potValue = analogRead(targetPressure);
	int pumpSwitchState = digitalRead(Switch_Pump_ON);

	if (mode == MainSwitchStates::Main_Switch_MANUAL)
	{
		if (pumpSwitchState == LOW)
		{
			if (potValue <= 250) return Scenarios::Scenario_1;
			if (potValue <= 500) return Scenarios::Scenario_2;
			if (potValue <= 750)return Scenarios::Scenario_3;
			return Scenarios::Scenario_4;
		}
		return Scenarios::Scenario_5;

	}
	if (mode == MainSwitchStates::Main_Switch_REMOTE)
	{
		if (currentScenario >= static_cast<int>(Scenarios::Scenario_1) && currentScenario <= static_cast<int>(Scenarios::Scenario_5))
		{
			return static_cast<Scenarios>(currentScenario);
		}
	}

	return Scenarios::Invalid_Scenario;
}

int VacControl::getScenarioFromPotValue(int potValue)
{
    if (potValue <= 250)  return static_cast<int>(Scenarios::Scenario_1);
    if (potValue <= 500)  return static_cast<int>(Scenarios::Scenario_2);
    if (potValue <= 750)  return static_cast<int>(Scenarios::Scenario_3);
    if (potValue <= 1023) return static_cast<int>(Scenarios::Scenario_4);

    return -1;
}

float VacControl::getCurrentTargetPressure()
{
	if (digitalRead(Main_Switch_OFF) == LOW)
		return 1000;

	if (digitalRead(Main_Switch_MANUAL) == LOW)
	{
		if (digitalRead(Switch_Pump_ON) == HIGH)
			return 1000;

		int potValue = analogRead(targetPressure);
		switch (getScenarioFromPotValue(potValue))
		{
			case Scenarios::Scenario_1: return 0;
			case Scenarios::Scenario_2: return TARGET_PRESSURE_1;
			case Scenarios::Scenario_3: return TARGET_PRESSURE_2;
			case Scenarios::Scenario_4: return TARGET_PRESSURE_3;
		}
	}
	if (digitalRead(Main_Switch_REMOTE) == LOW)
		return -1;
	return -1;
}

void VacControl::setVacuumLed(float pressure, float targetPressure)
{
	float dynamicTolerance = targetPressure * 0.2f;
	float lowerLimit = targetPressure - dynamicTolerance;
	float upperLimit = targetPressure + dynamicTolerance;

    if (pressure >= lowerLimit && pressure <= upperLimit)
    {
        digitalWrite(targetVacuumLED, HIGH);
    }
    else
    {
        digitalWrite(targetVacuumLED, LOW);
    }
}

void VacControl::setPump(bool flag)
{
	if (flag)
	{
		digitalWrite(Pump_Relay, HIGH);
		digitalWrite(Pump_Status_LED, HIGH);
		//SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Pump is running."));
	}
	else
	{
		digitalWrite(Pump_Relay, LOW);
		digitalWrite(Pump_Status_LED, LOW);
		//SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO,("Pump is OFF."));
	}
}

void VacControl::run()
{
	MainSwitchStates state = getMainSwitchState();

	switch (state)
	{
		case MainSwitchStates::Main_Switch_OFF:handleOffMode();break;
		case MainSwitchStates::Main_Switch_MANUAL:handleManualMode();break;
		case MainSwitchStates::Main_Switch_REMOTE:handleRemoteMode();break;
		case MainSwitchStates::Main_switch_INVALID:
		default:
			handleInvalidMode();
			break;
	}
}

void VacControl::handleOffMode()
{
	if (lastState != 0)
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is OFF"));
		lastState = 0;
	}

	setPump(false);
	digitalWrite(targetVacuumLED, LOW);
	currentScenario = static_cast<int>(Scenarios::Scenario_5);  // Szenario 5 aktivieren

	lastPumpState = -1;
}

void VacControl::handleManualMode()
{
    if (lastState != 1)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Manual Mode"));
        lastState = 1;
    }

    int pumpOnState = digitalRead(Switch_Pump_ON);
    int potValue = analogRead(targetPressure);
    Pressure currentMeasurement = measure();

    if (pumpOnState == LOW)
    {
        if (lastPumpState != 1)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Pump ON."));
            lastPumpState = 1;

            currentScenario = static_cast<int>(getScenario());

            float targetPressureValue = 0;

            switch (currentScenario)
            {
                case static_cast<int>(Scenarios::Scenario_1): break;
                case static_cast<int>(Scenarios::Scenario_2):targetPressureValue = TARGET_PRESSURE_1; break;
                case static_cast<int>(Scenarios::Scenario_3):targetPressureValue = TARGET_PRESSURE_2;break;
                case static_cast<int>(Scenarios::Scenario_4):targetPressureValue = TARGET_PRESSURE_3;break;
                default:
                    SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Scenario."));
                    setPump(false);
                    return;
            }

            setPump(true);
        }

        float currentTargetPressure = getCurrentTargetPressure();
        setVacuumLed(currentMeasurement.pressure, currentTargetPressure);
    }
    else if (pumpOnState == HIGH && lastPumpState != 2)
    {
        lastPumpState = 2;
        currentScenario = static_cast<int>(Scenarios::Scenario_5);
        setPump(false);
        digitalWrite(targetVacuumLED, LOW);
    }
}

void VacControl::handleRemoteMode()
{
	if (lastState != 2)
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Remote Mode"));
		lastState = 2;

		setPump(false);
		setExternPump(0);
		lastPumpState = 2;
	}

	setPump(getExternPump());
}

void VacControl::handleInvalidMode()
{
	if (lastState != 3)
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("VAC: Invalid Switch Position"));
		setPump(false);
		lastState = 3;
	}
}

void VacControl::externPump(int pumpState)
{
	digitalWrite(Pump_Relay, pumpState);
}

void VacControl::setExternPump(int state)
{
	if(state < 0 || state > 1)
	{
		return;
	}
	currentPumpState = state;
}

int VacControl::getExternPump()
{
	return currentPumpState;
}

void VacControl::setExternScenario(int scenario)
{
	if (scenario < 0 || scenario > 4)
	{
		return;
	}
	currentScenario = scenario;
}

int VacControl::getExternScenario()
{
	return currentScenario;
}

void VacControl::setExternPressure(float pressure)
{
	if (pressure < 0)
		return;
	meas.pressure = pressure;
}

float VacControl::getExternPressure()
{
	return meas.pressure;
}
