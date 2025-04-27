#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <VacControl.h>
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
	pinMode(Main_Switch_OFF, INPUT);		//Kraus-Naimer-Schalter OFF
	pinMode(Main_Switch_MANUAL, INPUT);		//Kraus-Naimer-Schalter MANUAL
	pinMode(Main_Switch_REMOTE, INPUT);		//Kraus-Naimer-Schalter REMOTE

	//Setup pinMode for Pump
	pinMode(Switch_Pump_ON, INPUT);			//TASTER PUMP ON
	pinMode(targetPressure, INPUT);			//POTI FOR Pressure Regulation
	pinMode(Pump_Relay, OUTPUT);			//Pumpen Relais
	pinMode(Pump_Status_LED, OUTPUT);		//Status LED für Pumpe
	pinMode(targetVacuumLED, OUTPUT);		//Status LED für Vakuum

	_vacControlInitialized = true;
}

bool VacControl::isInitialized() const
{
	return _vacControlInitialized;
}

SwitchStates VacControl::getSwitchState()
{
	if (digitalRead(Main_Switch_OFF) == HIGH)
	{
		return SwitchStates::Main_Switch_OFF;
	}
	else if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		return SwitchStates::Main_Switch_MANUAL;

		if(digitalRead(Switch_Pump_ON) == HIGH)
		{
			return SwitchStates::PUMP_ON;
		}
		else if(digitalRead(Switch_Pump_ON) == LOW)
		{
			return SwitchStates::PUMP_OFF;
		}
	}
	else if (digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		return SwitchStates::Main_Switch_REMOTE;
	}
	else
	{
		return SwitchStates::Main_switch_INVALID;
	}
}

Pressure VacControl::measure()
{
	if(digitalRead(Main_Switch_MANUAL) == HIGH || digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		meas.pressure = getExternPressure();  // Wert auslesen und speichern
	}
	return meas;
}

Scenarios VacControl::getScenario()
{
	if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		if(digitalRead(Switch_Pump_ON) == HIGH)
		{
			int potValue = analogRead(targetPressure);

			if(potValue >= 0 && potValue <= 250)
			{
				//Hier wird Szenario 1 dem Controller gesendet
				return Scenarios::Scenario_1;
			}
			else if(potValue >= 251 && potValue <= 500)
			{
				//Hier wird Szenario 2 dem Controller gesendet
				return Scenarios::Scenario_2;
			}
			else if(potValue >= 501 && potValue <= 750)
			{
				//Hier wird Szenario 3 dem Controller gesendet
				return Scenarios::Scenario_3;
			}
			else if(potValue >= 751 && potValue <= 1023)
			{
				//Hier wird Szenario 4 dem Controller gesendet
				return Scenarios::Scenario_4;
			}
		}
		else if(digitalRead(Switch_Pump_ON) == LOW)
		{
			return Scenarios::Scenario_5;
		}
	}

	return Scenarios::not_defined;
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
    int offState = digitalRead(Main_Switch_OFF);
    int manualState = digitalRead(Main_Switch_MANUAL);
    int remoteState = digitalRead(Main_Switch_REMOTE);
    int pumpOnState = digitalRead(Switch_Pump_ON);

    if (offState == HIGH)
    {
        if (lastState != 0)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is OFF"));
            lastState = 0;
        }
        setPump(false);
        digitalWrite(targetVacuumLED, LOW);
        return;
    }

    Pressure currentMeasurement = measure();
    int potValue = analogRead(targetPressure);

    if (manualState == HIGH)
    {
        if (lastState != 1)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Manual Mode"));
            lastState = 1;
        }

        if (pumpOnState == HIGH && lastPumpState != 1)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("Pump ON."));
            lastPumpState = 1;

            currentScenario = getScenarioFromPotValue(potValue);

            int targetPressureValue = 0;
            bool pumpState = true;

            switch(currentScenario)
            {
                case static_cast<int>(Scenarios::Scenario_1): targetPressureValue = TARGET_PRESSURE_1; pumpState = false; break;
                case static_cast<int>(Scenarios::Scenario_2): targetPressureValue = TARGET_PRESSURE_2; break;
                case static_cast<int>(Scenarios::Scenario_3): targetPressureValue = TARGET_PRESSURE_3; break;
                case static_cast<int>(Scenarios::Scenario_4): targetPressureValue = TARGET_PRESSURE_4; break;
                default:
                	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Scenario."));
                    setPump(false);
                    return;
            }

            setVacuumLed(currentMeasurement.pressure, targetPressureValue);
            setPump(pumpState);
        }
        else if (pumpOnState == LOW && lastPumpState != 2)
        {
            lastPumpState = 2;
            currentScenario = static_cast<int>(Scenarios::Scenario_5);
            setPump(false);
            digitalWrite(targetVacuumLED, LOW);
        }
    }
    else if (remoteState == HIGH)
    {
        if (lastState != 2)
        {
            SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Remote Mode"));
            lastState = 2;

            int scenario = getExternScenario();
            if (scenario < static_cast<int>(Scenarios::Scenario_1) || scenario > static_cast<int>(Scenarios::Scenario_5))
            {
                SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Scenario"));
            }

            // TODO Addd the additional logic here!!!!
        }
    }
    else
    {
    	if (lastState != 3)
    	{
    		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Switch Position"));
    		lastState = 3;
    	}
    }
}

void VacControl::setExternScenario(int scenario)
{
	if (scenario < 0 || scenario > 5)
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
	meas.pressure = pressure;
}

float VacControl::getExternPressure()
{
	return meas.pressure;
}
