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
Flyback::~Flyback() {}

/// @brief Initializes the Flyback module
void Flyback::initialize()
{
	//Setup pinMode for Main_Switch
	pinMode(Main_Switch_OFF, INPUT);
	pinMode(Main_Switch_HAND, INPUT);
	pinMode(Main_Switch_REMOTE, INPUT);
	pinMode(HV_Controller, INPUT);
	pinMode(Measure_ADC, INPUT);

	//Setup pinMode for Indicator LED
	pinMode(HV_Module_ON, OUTPUT);
	pinMode(HV_Module_OFF, OUTPUT);
	pinMode(HV_Module_Working, OUTPUT);

	//Setup Timer for Flyback
	timerConfig();

	_flybackInitialized = true;
	reportError("Flyback module initialized");
}

/// @brief Function to configure the timer settings
void Flyback::timerConfig()
{
	//Setup Timer for Flyback
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // No prescaler
	ICR1 = 16000000 / 1000;  // Default frequency at 1kHz
	OCR1A = ICR1 / 2;         // 50% duty cycle

	_timerInitialized = true;
}

/// @brief Checks if the Flyback module is initialized
/// @return bool -> Returns true if the Flyback module is initialized, otherwise false
bool Flyback::isInitialized() const
{
	return _flybackInitialized;
}

/// @brief Function to change the PWM frequency
/// @param frequency -> The new PWM frequency
/// @return bool -> Returns true if the temperature sensor is initialized, otherwise false
void Flyback::SetPWMFrequency(int frequency)
{
    // Calculate ICR1 based on the desired frequency
    ICR1 = 16000000 / frequency;
    OCR1A = ICR1 / 2; // Set duty cycle to 50%
}

/// @brief Returns the state of the timer
/// @return bool -> Returns true if the timer is initialized, otherwise false
bool Flyback::getTimerState()
{
	return _timerInitialized;
}

/// @brief Sets the state of the timer
/// @param state -> If true, the timer will be enabled, otherwise disabled
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

/// @brief Returns the state of the Main-Switch
/// @return String -> The current state of the Main-Switch (e.g., "HV_Module OFF", "HV_Module HAND", "HV_Module REMOTE", "Invalid Switch Position")
String Flyback::getSwitchState()
{
	if(digitalRead(Main_Switch_OFF) == HIGH)
	{
		return "HV_Module OFF";

	}
	else if(digitalRead(Main_Switch_HAND) == HIGH)
	{
		return "HV_Module HAND";
	}
	else if(digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		return "HV_Module REMOTE";
	}
	else
	{
		return "Invalid Switch Position";
	}
}

/// @brief Prints an error message via the serial connection
/// @param errorMessage -> The error message to be printed
void Flyback::reportError(const char* errorMessage)
{
	Serial.print(F("Error: "));
	Serial.println(errorMessage);
}

/// @brief Measures the voltage, current, power, digitalValue and frequency of the system
/// @return Measurement -> A Measurement object containing voltage, current, and power
Measurement Flyback::measure()
{
	int adcValue = analogRead(Measure_ADC);

	// Wird kontinuierlich Bearbeitet
	//float reducedVoltage = adcValue * (Vcc / ADC_Max_Value);

	// Testspannung für Simulation!
	float reducedVoltage = 1.25;

	// Berechne die Strom
	float current = (reducedVoltage / R2) * 1000000;
	// Berechne Spannung
	float voltage = (current / 1000000) * R1;
	// Berechne Leistung
	float power = (voltage * current) / 1000000;
	// Digitalwert des Potentiometers auslesen
	int digitalValue = analogRead(HV_Controller);
	// Frequenz berechnen (basierend auf dem Potentiometerwert)
	int frequency = map(digitalValue, 0, 1023, 1000, 25000);

	// Für Regelkreis des Flybackmoduls eventuell Interrupt-Routine einfügen um die tatsächliche Frequenz am PIN 9 OCR1A zu messen
	// Neuer Task den man ausarbeiten kann


	return {voltage, current, power, digitalValue, frequency};
}

/// @brief Executes logic depending on which Main-Switch state is active
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
		reportError("System is OFF");
	}
	else if(digitalRead(Main_Switch_HAND)==HIGH)
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
		SetPWMFrequency(frequency);


		//Error-Meldung
		reportError("System is ON - Manual Mode");
	}
	else if(digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		//Hier soll die Logik von dem HAS kommen
		//Frequenz soll Remote verstellt werden
		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		//Error-Meldung
		reportError("System is ON - Remote Mode");
	}
	else
	{
		reportError("Invalid Switch Position");
		digitalWrite(HV_Module_Working, LOW);
		digitalWrite(HV_Module_ON, LOW);
		digitalWrite(HV_Module_OFF, LOW);
	}

}








