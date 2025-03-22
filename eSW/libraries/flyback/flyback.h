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

/// @brief Namespace for the Flyback module \namespace flybackModule
namespace flybackModule
{
	enum class SwitchStates : int
	{
		HV_Module_OFF,
		HV_Module_MANUAL,
		HV_Module_REMOTE,
		HV_Module_INVALID
	};
	/// @brief Structure to store the measured values of the system \struct Measurement
	/// This structure holds the voltage, current, and power values measured from the system.
	typedef struct Measurement
	{
		float voltage;	//measured Voltage
		float current;	//measured Current
		float power;	//measured power
		int digitalValue; //measured DigitalValue for Frequency 0--1023
		int frequency;	//current PWM - Frequency
	} meas;

	/// @brief Flyback class to manage the Flyback system \class Flyback
	/// This class provides methods for initializing the system, configuring the timer,
	/// measuring parameters, and handling different system states such as ON, OFF, HAND, and REMOTE modes.
	class Flyback
	{
	public:
		Flyback();
		~Flyback();

        /**
         * @brief Initialize the Flyback system
         * 
         */
		void initialize();
        
        /**
         * @brief Get the state of the Flyback system
         * 
         * @return true -> Flyback is initialized
         * @return false -> Flyback is not initialized
         */
		bool isInitialized() const;

        /**
         * @brief Returns the state of the timer
         * 
         * @return true -> if the timer is initialized
         * @return false -> if the timer is not initialized
         */
		bool getTimerState();

        /**
         * @brief Sets the state of the timer
         * 
         * @param state -> If true, the timer will be enabled, otherwise disabled
         */
		void setTimerState(bool state);

        /**
         * @brief Get the state of the Main-Switch
         * 
         * @return Enum -> The current state of the Main-Switch
         * (e.g., "HV_Module OFF", "HV_Module MANUAL", "HV_Module REMOTE", "Invalid Switch Position")
         */
		SwitchStates getSwitchState();

        /**
         * @brief Measures the voltage, current, power, digitalValue and frequency of the system
         * 
         * @return Measurement -> A Measurement object containing voltage, current, and power
         */
		Measurement measure();

        /**
         * @brief Executes logic depending on which Main-Switch state is active
         * 
         */
		void run();

		/**
		 * @brief Function to get the desired Frequency from HAS
		 * @param frequency -> the frequency to change
		 *
		 */
		void setExternFrequency(int frequency);

		/**
		 * @brief Getter Function to get the Frequency
		 *
		 */
		int getExternFrequency();


	private:
		Measurement meas;

		//Define Pins -->Signaltable
		static const int Main_Switch_OFF = 27;		//Schalterposition OFF 27
		static const int Main_Switch_MANUAL = 28;		//Schalterposition Handsteuerung 28
		static const int Main_Switch_REMOTE = 29;	//Schalterposition Remote 29
		static const int Measure_ADC = A0;			//ADC Pin für die Messung
		static const int PWM_OUT = 11;

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

		int externFrequency = 0;

		bool _flybackInitialized;
		bool _timerInitialized;


        /**
         * @brief Function to configure the timer settings
         * 
         */
		void timerConfig();


		// Sensibility
		int steps = 20; // Divide into 20 discrete steps

        /**
         * @brief Function to change the PWM frequency
         * 
         * @param frequency -> The new PWM frequency
         */
		void setPWMFrequency(int frequency);

        /**
         * @brief Prints an error message via the serial connection
         * 
         * @param errorMessage -> The error message to be printed
         */
		void reportError(const char* errorMessage);
	};
}

#endif //FLYBACK_H
