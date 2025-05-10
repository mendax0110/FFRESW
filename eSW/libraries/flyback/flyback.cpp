#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <flyback.h>
#include <serialMenu.h>

using namespace flybackModule;

SwitchStates Flyback::lastState = SwitchStates::HV_Module_INVALID;
bool Flyback::lastTimerState = false;
int Flyback::lastPWMFrequency = 0;
int Flyback::lastPWMDutyCycle = 0;


Flyback::Flyback()
{
	_flybackInitialized = false;
	_timerInitialized = false;
}


Flyback::~Flyback()
{

}


void Flyback::initialize()
{
	//Setup pinMode for Main_Switch
	pinMode(Main_Switch_OFF, INPUT);
	pinMode(Main_Switch_MANUAL, INPUT);
	pinMode(Main_Switch_REMOTE, INPUT);
	pinMode(PWM_Frequency, INPUT);
	pinMode(PWM_DutyCycle, INPUT);
	pinMode(Measure_ADC, INPUT);

	//Setup pinMode for Indicator LED
	pinMode(HV_Module_ON, OUTPUT);
	pinMode(HV_Module_OFF, OUTPUT);
	pinMode(HV_Module_Working, OUTPUT);
	pinMode(PWM_OUT, OUTPUT);
	pinMode(PWM_INV, OUTPUT);

	//Setup Timer for Flyback
	timerConfig();

	_flybackInitialized = true;
	SerialMenu::printToSerial(F("[INFO] Flyback module initialized."));
}

void Flyback::deinitialize()
{
	//Setup pinMode for Main_Switch
    pinMode(Main_Switch_OFF, INPUT);
    pinMode(Main_Switch_MANUAL, INPUT);
    pinMode(Main_Switch_REMOTE, INPUT);
    pinMode(PWM_Frequency, INPUT);
    pinMode(PWM_DutyCycle, INPUT);
    pinMode(Measure_ADC, INPUT);

    //Setup pinMode for Indicator LED
    pinMode(HV_Module_ON, INPUT);
    pinMode(HV_Module_OFF, INPUT);
    pinMode(HV_Module_Working, INPUT);
    pinMode(PWM_OUT, INPUT);
    pinMode(PWM_INV, INPUT);


    digitalWrite(HV_Module_ON, LOW);
    digitalWrite(HV_Module_OFF, LOW);
    digitalWrite(HV_Module_Working, LOW);
    digitalWrite(PWM_OUT, LOW);
    digitalWrite(PWM_INV, LOW);

    _flybackInitialized = false;
    SerialMenu::printToSerial(F("[INFO] Flyback module deinitialized."));
}

void Flyback::timerConfig()
{
	// Setup Timer1 für Fast PWM, Top = ICR1
	TCCR1A = (1 << COM1A1) | (0 << COM1A0) | // OC1A: Nicht-invertiertes PWM
             (1 << COM1B1) | (1 << COM1B0) | // OC1B: Invertiertes PWM
			 (1 << WGM11);                   // Fast PWM, Teil 1
	TCCR1B = (1 << WGM13) | (1 << WGM12) |  // Fast PWM, Top = ICR1
			 (1 << CS10);                   // Kein Prescaler (16 MHz)
	    _timerInitialized = true;
}

bool Flyback::isInitialized() const
{
	return _flybackInitialized;
}

void Flyback::setPWMFrequency(uint32_t frequency, int dutyCycle)
{
	ICR1 = static_cast<uint16_t>(16000000 / frequency); // Setze Top-Wert für Frequenz
	uint16_t ocrValue = (ICR1 * dutyCycle) / 100;       // Berechne Duty Cycle
	OCR1A = ocrValue;                                   // OC1A (Pin 11, nicht-invertiert)
	OCR1B = ocrValue; 						 			// Calculate duty cycle based on the desired duty Cycle
}

bool Flyback::getTimerState()
{
	return _timerInitialized;
}

void Flyback::setTimerState(bool state)
{
	if(state && !lastTimerState)
	{
		// Turn timer off
		timerConfig();
	}
	else if (!state && lastTimerState)
	{
		// Turn timer off
		TCCR1B &= ~(1 << CS10);
		_timerInitialized = false;
	}
	lastTimerState = state; // update the last settet timer state
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

Measurement Flyback::measure()
{
	int adcValue = analogRead(Measure_ADC);

	// Wird kontinuierlich Bearbeitet
	//float reducedVoltage = adcValue * (Vcc / ADC_Max_Value);

	// Testing Voltage for simulation!
	float reducedVoltage = 1.25;
	meas.current = (reducedVoltage / R2) * 1000000;
	meas.voltage = (meas.current / 1000000) * R1;
	meas.power = (meas.voltage * meas.current) / 1000000;

	if (digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		// Read in digitValue from poti
		meas.digitalFreqValue = analogRead(PWM_Frequency);
		meas.digitalDutyValue = analogRead(PWM_DutyCycle);

		// Maping out the digitValues
		meas.frequency = map(meas.digitalFreqValue, 0, 1023, 25000, 100000);
		meas.dutyCycle = map(meas.digitalDutyValue, 0, 1023, 1, 50);
	}
	else if (digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		uint32_t freq = getExternFrequency();
		int dutycycle = getExternDutyCycle();
		setPWMFrequency(freq, dutycycle);
	}
    return meas;
}

void Flyback::run()
{
	int offState = digitalRead(Main_Switch_OFF);
	int manualState = digitalRead(Main_Switch_MANUAL);
	int remoteState = digitalRead(Main_Switch_REMOTE);

	if (offState == HIGH)
	{
		if (lastState != SwitchStates::HV_Module_OFF)
		{
			SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is OFF."));
			lastState = SwitchStates::HV_Module_OFF;
		}

		if (getTimerState())
		{
			setTimerState(false);
		}

		digitalWrite(HV_Module_OFF, HIGH);
		digitalWrite(HV_Module_Working, LOW);
		digitalWrite(HV_Module_ON, LOW);
	}
	else if (manualState == HIGH)
	{
		if (lastState != SwitchStates::HV_Module_MANUAL)
		{
			SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Manual Mode."));
			lastState = SwitchStates::HV_Module_MANUAL;
		}

		if (!getTimerState())
		{
			setTimerState(true);
		}

		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		// PWM frequency and duty cycle adjustments
		int potFreqValue = analogRead(PWM_Frequency);
		int potDutyValue = analogRead(PWM_DutyCycle);
		uint32_t frequency = map(potFreqValue, 0, 1023, 25000, 100000);
		int dutyCycle = map(potDutyValue, 0, 1023, 1, 50);

		if (frequency != lastPWMFrequency || dutyCycle != lastPWMDutyCycle)
		{
			setPWMFrequency(frequency, dutyCycle);
			lastPWMFrequency = frequency;
			lastPWMDutyCycle = dutyCycle;
		}
	}
	else if (remoteState == HIGH)
	{
		if (lastState != SwitchStates::HV_Module_REMOTE)
		{
			SerialMenu::printToSerial(SerialMenu::OutputLevel::INFO, F("System is ON - Remote Mode."));
			lastState = SwitchStates::HV_Module_REMOTE;
		}

		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		// Set PWM frequency and duty cycle from external settings
		setPWMFrequency(getExternFrequency(), getExternDutyCycle());
	}
	else
	{
		if (lastState != SwitchStates::HV_Module_INVALID)
		{
			SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Invalid Switch Position."));
			lastState = SwitchStates::HV_Module_INVALID;
			setTimerState(false);
			digitalWrite(HV_Module_Working, LOW);
			digitalWrite(HV_Module_ON, LOW);
			digitalWrite(HV_Module_OFF, LOW);
		}
	}
}

uint32_t Flyback::getExternFrequency()
{
	return meas.frequency;
}

void Flyback::setExternFrequency(uint32_t frequency)
{
	if (frequency < 25000 || frequency > 100000)
	{
		return;
	}
	meas.frequency = frequency;
}

int Flyback::getExternDutyCycle()
{
	return meas.dutyCycle;
}

void Flyback::setExternDutyCycle(int dutyCycle)
{
	if (dutyCycle < 1 || dutyCycle > 50)
	{
		return;
	}
	meas.dutyCycle = dutyCycle;
}
