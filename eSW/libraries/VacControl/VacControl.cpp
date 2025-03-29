#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <VacControl.h>

using namespace vacControl;

// COnstructor for the
vacControl::vacControl()
{
	_vacControlInitialized = false;
}

vacControl::~vacControl()
{

}

void vacControl::initialize()
{
	//Setup pinMode for Main_Switch
	pinMode(Main_Switch_OFF, INPUT);		//Kraus-Naimer-Schalter OFF
	pinMode(Main_Switch_MANUAL, INPUT);		//Kraus-Naimer-Schalter MANUAL
	pinMode(Main_Switch_REMOTE, INPUT);		//Kraus-Naimer-Schalter REMOTE
	pinMode(Switch_Pump_ON, INPUT);				//TASTER PUMP ON
	pinMode(Switch_Pump_OFF, INPUT);				//TASTER PUMP OFF
	pinMode(targetPressure, INPUT);			//POTI FOR Pressure Regulation
}

bool vacControl::isInitialized() const
{
	return _vacControlInitialized();
}

Measurement vacControl::measure()
{

	return meas;
}

SwitchStates vacControl::getSwitchState()
{
	if (digitalRead(Main_Switch_OFF) == HIGH)
	{
		return SwitchStates::Main_Switch_OFF;
	}
	else if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		return SwitchStates::Main_Switch_MANUAL;
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

void vacControl::run()
{
	if(digitalRead(Main_Switch_OFF) == HIGH)
	{
		//Error-Meldung
		reportError("[INFO] System is OFF");
	}
	else if(digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		//Error-Meldung
		reportError("[INFO] System is ON - Manual Mode");

		if(digitalRead(PUMP_ON) == HIGH)
		{
			reportError("[INFO] PUMP_ON");

		}
		else if(digitalRead(PUMP_OFF) == HIGH)
		{
			reportError("[INFO] PUMP_OFF");
		}

	}
	else if(digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		//Error-Meldung
		reportError("[INFO] System is ON - Remote Mode");
	}
	else
	{
		//Error-Meldung
		reportError("[ERROR] Invalid Switch Position");
	}
}

void vacControl::reportError(const char* errorMessage)
{
	Serial.println(errorMessage);
}






