#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <flyback.h>

using namespace flybackModule;


// Constructor for the Flyback class
Flyback::Flyback()
{
	_flybackInitialized = false;
	_timerInitialized = false;
}

// Destructor for the Flyback class
Flyback::~Flyback()
{

}

/// @brief Initializes the Flyback module
void Flyback::initialize()
{
	//Setup pinMode for Main_Switch
	pinMode(Main_Switch_OFF, INPUT);
	pinMode(Main_Switch_MANUAL, INPUT);
	pinMode(Main_Switch_REMOTE, INPUT);
	pinMode(HV_Controller, INPUT);
	pinMode(Measure_ADC, INPUT);

	//Setup pinMode for Indicator LED
	pinMode(HV_Module_ON, OUTPUT);
	pinMode(HV_Module_OFF, OUTPUT);
	pinMode(HV_Module_Working, OUTPUT);
	pinMode(PWM_OUT, OUTPUT);

	//Setup Timer for Flyback
	timerConfig();

	_flybackInitialized = true;
	reportError("[INFO] Flyback module initialized");
}

void Flyback::timerConfig()
{
	//Setup Timer for Flyback
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // No prescaler
	ICR1 = 16000000 / 1000;  // Default frequency at 1kHz
	OCR1A = ICR1 / 2;         // 50% duty cycle

	_timerInitialized = true;
}

bool Flyback::isInitialized() const
{
	return _flybackInitialized;
}

void Flyback::setPWMFrequency(int frequency)
{
    // Calculate ICR1 based on the desired frequency
    ICR1 = 16000000 / frequency;
    OCR1A = ICR1 / 2; // Set duty cycle to 50%
}

bool Flyback::getTimerState()
{
	return _timerInitialized;
}

void Flyback::setTimerState(bool state)
{
	if(state)
	{
		// Timer aktiveiren
		timerConfig();
	}
	else
	{
		// Timer deaktivieren
		TCCR1B &= ~(1 << CS10);
		_timerInitialized = false;
	}
}

SwitchStates Flyback::getSwitchState()
{
	if (digitalRead(Main_Switch_OFF) == HIGH)
	{
		return SwitchStates::HV_Module_OFF;
	}
	else if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		return SwitchStates::HV_Module_MANUAL;
	}
	else if (digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		return SwitchStates::HV_Module_REMOTE;
	}
	else
	{
		return SwitchStates::HV_Module_INVALID;
	}
}

void Flyback::reportError(const char* errorMessage)
{
	Serial.println(errorMessage);
}

Measurement Flyback::measure()
{
	int adcValue = analogRead(Measure_ADC);

	// Wird kontinuierlich Bearbeitet
	//float reducedVoltage = adcValue * (Vcc / ADC_Max_Value);

	// Testspannung für Simulation!
	float reducedVoltage = 1.25;

	// Berechne die Strom
	meas.current = (reducedVoltage / R2) * 1000000;
	// Berechne Spannung
	meas.voltage = (meas.current / 1000000) * R1;
	// Berechne Leistung
	meas.power = (meas.voltage * meas.current) / 1000000;

	if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		// Digitalwert des Potentiometers auslesen
		meas.digitalValue = analogRead(HV_Controller);
		// Frequenz berechnen (basierend auf dem Potentiometerwert)
		meas.frequency = map(meas.digitalValue, 0, 1023, 1000, 25000);
	}
	else if (digitalRead(Main_Switch_REMOTE)==HIGH)
	{
		int freq = getExternFrequency();
		setPWMFrequency(freq);
	}


	// Für Regelkreis des Flybackmoduls eventuell Interrupt-Routine einfügen um die tatsächliche Frequenz am PIN 9 OCR1A zu messen
	// Neuer Task den man ausarbeiten kann

    return meas;
}

void Flyback::run()
{
	if(digitalRead(Main_Switch_OFF) == HIGH)
	{
		if(getTimerState())
		{
			setTimerState(false);
		}

		//LED Status setzen
		digitalWrite(HV_Module_OFF, HIGH);
		digitalWrite(HV_Module_Working, LOW);
		digitalWrite(HV_Module_ON, LOW);

		//Error-Meldung
		reportError("[INFO] System is OFF");
	}
	else if(digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		if(!getTimerState())
		{
			setTimerState(true);
		}
		//LED Status setzen
		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		// PWM-Frequnz mit Potentiometer steuern
		int potValue = analogRead(HV_Controller);

		// Umrechnung von Potentiometer-Wert auf Frequenzbereich
		int frequency = map(potValue, 0, 1023, 1000, 25000);		//Methode

		//Frequenz in den Timer eintragen
		setPWMFrequency(frequency);


		//Error-Meldung
		reportError("[INFO] System is ON - Manual Mode");
	}
	else if(digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		//Hier soll die Logik von dem HAS kommen
		//Frequenz soll Remote verstellt werden
		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		setPWMFrequency(getExternFrequency());

		//Error-Meldung
		reportError("[INFO] System is ON - Remote Mode");
	}
	else
	{
		reportError("[ERROR] Invalid Switch Position");
		setTimerState(false);
		digitalWrite(HV_Module_Working, LOW);
		digitalWrite(HV_Module_ON, LOW);
		digitalWrite(HV_Module_OFF, LOW);
	}
}

int Flyback::getExternFrequency()
{
	return meas.frequency;
}

void Flyback::setExternFrequency(int frequency)
{
	if (frequency < 0 || frequency > 25000)
	{
		return;
	}
	meas.frequency = frequency;
}
