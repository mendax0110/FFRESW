/**
 * @file vacControl.cpp
 * @brief Implementation of the vacControl class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
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
	//Setup pinMode for Main_Switch
	pinMode(Main_Switch_OFF, INPUT_PULLUP);			//Kraus-Naimer-Schalter OFF
	pinMode(Main_Switch_MANUAL, INPUT_PULLUP);		//Kraus-Naimer-Schalter MANUAL
	pinMode(Main_Switch_REMOTE, INPUT_PULLUP);		//Kraus-Naimer-Schalter REMOTE

	//Setup pinMode for Pump
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
    // reset pinMode for Main_Switch
    pinMode(Main_Switch_OFF, INPUT);
    pinMode(Main_Switch_MANUAL, INPUT);
    pinMode(Main_Switch_REMOTE, INPUT);

    // reset pinMode for Pump
    pinMode(Switch_Pump_ON, INPUT);
    pinMode(targetPressure, INPUT);
    pinMode(Pump_Relay, INPUT);
    pinMode(Pump_Status_LED, INPUT);
    pinMode(targetVacuumLED, INPUT);

    // addtioninal cleanup
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
    if (digitalRead(Main_Switch_OFF) == LOW)
        return MainSwitchStates::Main_Switch_OFF;

    if (digitalRead(Main_Switch_MANUAL) == LOW)
        return MainSwitchStates::Main_Switch_MANUAL;

    if (digitalRead(Main_Switch_REMOTE) == LOW)
        return MainSwitchStates::Main_Switch_REMOTE;

    return MainSwitchStates::Main_switch_INVALID;
}

PumpSwitchStates VacControl::getPumpSwitchState()
{
    if (digitalRead(Main_Switch_MANUAL) == LOW && digitalRead(Switch_Pump_ON) == LOW)
        return PumpSwitchStates::Switch_Pump_ON;

    return PumpSwitchStates::Switch_Pump_OFF;
}

PumpState VacControl::getPumpState()
{
    if (digitalRead(Pump_Relay) == HIGH)
        return PumpState::pump_ON;
    else
        return PumpState::pump_OFF;
}


Pressure VacControl::measure()
{
	if(digitalRead(Main_Switch_MANUAL) == LOW || digitalRead(Main_Switch_REMOTE) == LOW)
	{
		meas.pressure = getExternPressure();  // Wert auslesen und speichern
	}
	return meas;
}

Scenarios VacControl::getScenario()
{
	MainSwitchStates mode = getMainSwitchState();

	if (mode == MainSwitchStates::Main_Switch_MANUAL)
	{
		int pumpSwitchState = digitalRead(Switch_Pump_ON);
		if (pumpSwitchState == LOW)
		{
			int potValue = analogRead(targetPressure);

			if (potValue <= 250)
				return Scenarios::Scenario_1;
			else if (potValue <= 500)
				return Scenarios::Scenario_2;
			else if (potValue <= 750)
				return Scenarios::Scenario_3;
			else
				return Scenarios::Scenario_4;
		}
		else
		{
			return Scenarios::Scenario_5;
		}
	}
	else if (mode == MainSwitchStates::Main_Switch_REMOTE)
	{
		// Verwende extern gesetzten Wert
		if (currentScenario >= static_cast<int>(Scenarios::Scenario_1) && currentScenario <= static_cast<int>(Scenarios::Scenario_5))
		{
			return static_cast<Scenarios>(currentScenario);
		}
	}

	return Scenarios::Invalid_Scenario;
}


void VacControl::setVacuumLed(float pressure, float targetPressure)
{
	if (pressure <= targetPressure)
	{
		digitalWrite(targetVacuumLED, HIGH);
	}
	else
	{
		digitalWrite(targetVacuumLED, LOW);
	}
}

int VacControl::getScenarioFromPotValue(int potValue)
{
	if (potValue >= 0 && potValue <= 250)
		return static_cast<int>(Scenarios::Scenario_1);
	else if (potValue >= 251 && potValue <= 500)
		return static_cast<int>(Scenarios::Scenario_2);
	else if (potValue >= 501 && potValue <= 750)
		return static_cast<int>(Scenarios::Scenario_3);
	else if (potValue >= 751 && potValue <= 1023)
		return static_cast<int>(Scenarios::Scenario_4);

	return -1; // Ungültiges Szenario
}

void VacControl::setPump(bool flag)
{
	if (flag)
	{
		digitalWrite(Pump_Relay, HIGH);
		digitalWrite(Pump_Status_LED, HIGH);
		SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Pump is running."));
	}
	else
	{
		digitalWrite(Pump_Relay, LOW);
		digitalWrite(Pump_Status_LED, LOW);
		SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO,("Pump is OFF."));
	}
}

void VacControl::run()
{
	MainSwitchStates state = getMainSwitchState();

	switch (state)
	{
		case MainSwitchStates::Main_Switch_OFF:
			handleOffMode();
			break;

		case MainSwitchStates::Main_Switch_MANUAL:
			handleManualMode();
			break;

		case MainSwitchStates::Main_Switch_REMOTE:
			handleRemoteMode();
			break;

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

    if (pumpOnState == LOW && lastPumpState != 1)
    {
        SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Pump ON."));
        lastPumpState = 1;

        currentScenario = static_cast<int>(getScenario());

        float targetPressureValue = 0;

        switch (currentScenario)
        {
            case static_cast<int>(Scenarios::Scenario_1):
                targetPressureValue = TARGET_PRESSURE_1;
                break;
            case static_cast<int>(Scenarios::Scenario_2):
                targetPressureValue = TARGET_PRESSURE_2;
                break;
            case static_cast<int>(Scenarios::Scenario_3):
                targetPressureValue = TARGET_PRESSURE_3;
                break;
            case static_cast<int>(Scenarios::Scenario_4):
                targetPressureValue = TARGET_PRESSURE_4;
                break;
            default:
                SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Scenario."));
                setPump(false);
                return;
        }

        setVacuumLed(currentMeasurement.pressure, targetPressureValue);
        setPump(true); // Pumpe EIN für Scenarios 1-4
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
	}

	// Nur die Pumpe wird über externen Wert gesteuert
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
