/*
 * flyback.h
 *
 *  Created on: 07.12.2024
 *      Author: domin
 */
#ifndef FLYBACK_H
#define FLYBACK_H

#include <Arduino.h>
#include <Wire.h>

namespace flybackModule
{
	/// @brief Structure to store the measured values of the system
	/// This structure holds the voltage, current, and power values measured from the system.
	struct Measurement
	{
		float voltage;	//measured Voltage
		float current;	//measured Current
		float power;	//measured power
		int digitalValue; //measured DigitalValue for Frequency 0--1023
		int frequency;	//current PWM - Frequency
	};

	/// @brief Flyback class to manage the Flyback system
	/// This class provides methods for initializing the system, configuring the timer,
	/// measuring parameters, and handling different system states such as ON, OFF, HAND, and REMOTE modes.
	class Flyback
	{
	public:
		Flyback();
		~Flyback();

		void initialize(); //Initialize Flyback
		bool isInitialized() const; //Initialize Flyback
		bool getTimerState();
		void setTimerState(bool state);
		String getSwitchState();
		Measurement measure();
		void run();

	private:

		//Define Pins -->Signaltable
		static const int Main_Switch_OFF = 27;		//Schalterposition OFF 27
		static const int Main_Switch_HAND = 28;		//Schalterposition Handsteuerung 28
		static const int Main_Switch_REMOTE = 29;	//Schalterposition Remote 29
		static const int Measure_ADC = A0;			//ADC Pin für die Messung

		//Variabeln für die Berechnung der Spannung
		const float R1 = 100000000; 				//Widerstand R1 (100 MΩ)
		const float R2 = 10000;						//Widerstand R2 (10 kΩ)
		const float ADC_Max_Value = 1023.0;			//Maximale ADC-Wert (10Bit)
		const float Vcc = 5.0;						//Referenzspannung des ATMega2560

		//Define Pins --> Signaltable
		static const int HV_Module_ON = 37;			//Button to turn HV-Module ON 37
		static const int HV_Module_OFF = 36;		//Button to turn HV-Module OFF 36
		static const int HV_Module_Working = 35;	//Indicator LED 35
		static const int HV_Controller = A1;		//Potentiometer for Regulation


		bool _flybackInitialized;
		bool _timerInitialized;
		void timerConfig();


		// Sensibility
		int steps = 20; // Divide into 20 discrete steps
		void SetPWMFrequency (int frequency);
		void reportError(const char* errorMessage);
	};
}

#endif //FLYBACK_H
