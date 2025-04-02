#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <flyback.h>

using namespace flybackModule;


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

	_timerInitialized = true;
}

bool Flyback::isInitialized() const
{
	return _flybackInitialized;
}

void Flyback::setPWMFrequency(uint32_t frequency, int dutyCycle)
{
    ICR1 = static_cast<uint16_t>(16000000 / frequency);		 // Calculate ICR1 based on the desired frequency
    OCR1A = (ICR1 * dutyCycle) / 100; 						 // Calculate duty cycle based on the desired duty Cycle
}

bool Flyback::getTimerState()
{
	return _timerInitialized;
}

void Flyback::setTimerState(bool state)
{
	if(state)
	{
		// Turn timer off
		timerConfig();
	}
	else
	{
		// Turn timer off
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
		meas.dutyCycle = map(meas.digitalDutyValue, 0, 1023, 10, 99);
	}
	else if (digitalRead(Main_Switch_REMOTE)==HIGH)
	{
		uint32_t freq = getExternFrequency();
		int dutycycle = getExternDutyCycle();
		setPWMFrequency(freq, dutycycle);
	}
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

		//Setting up LED based on state
		digitalWrite(HV_Module_OFF, HIGH);
		digitalWrite(HV_Module_Working, LOW);
		digitalWrite(HV_Module_ON, LOW);

		reportError("[INFO] System is OFF");
	}
	else if(digitalRead(Main_Switch_MANUAL) == HIGH)
	{
		if(!getTimerState())
		{
			setTimerState(true);
		}
		//Setting up LED based on state
		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		// Read in digitValue from poti
		int potFreqValue = analogRead(PWM_Frequency);
		int potDutyValue = analogRead(PWM_DutyCycle);
		// Mapping out the digitValues
		uint32_t frequency = map(potFreqValue, 0, 1023, 25000, 100000);
		int dutyCycle = map(potDutyValue, 0, 1023, 10, 99);

		setPWMFrequency(frequency, dutyCycle);

		reportError("[INFO] System is ON - Manual Mode");
	}
	else if(digitalRead(Main_Switch_REMOTE) == HIGH)
	{
		digitalWrite(HV_Module_Working, HIGH);
		digitalWrite(HV_Module_ON, HIGH);
		digitalWrite(HV_Module_OFF, LOW);

		setPWMFrequency(getExternFrequency(), getExternDutyCycle());

		reportError("[INFO] System is ON - Remote Mode");

		//TODO Implement method to update voltage with calculated values for frequency and dutycycle
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
	if (dutyCycle < 10 || dutyCycle > 99)
	{
		return;
	}
	meas.dutyCycle = dutyCycle;
}
