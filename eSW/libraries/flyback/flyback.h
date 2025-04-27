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
	/// @brief enum for different SwitchStates of HVModule \enum SwitchStates
	enum class SwitchStates : int
	{
		HV_Module_OFF,
		HV_Module_MANUAL,
		HV_Module_REMOTE,
		HV_Module_INVALID
	};

	/// @brief Structure to store the measured values of the system \struct Measurement
	/// This structure holds the voltage, current, power, frequency and dutycycle values measured from the system.
	typedef struct Measurement
	{
		float voltage;
		float current;
		float power;
		int digitalFreqValue;
		int digitalDutyValue;
		int dutyCycle;
		uint32_t frequency;
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
		void setExternFrequency(uint32_t frequency);

		/**
		 * @brief Getter Function to get the Frequency
		 *
		 */
		uint32_t getExternFrequency();

		/**
		 * @brief Function to get the desired DutyCycle from HAS
		 * @param dutyCycle -> the dutyCycle to change
		 *
		 */
		void setExternDutyCycle(int dutyCycle);

		/**
		 * @brief Getter Function to get the DutyCycle
		 * 			 *
		 */
		int getExternDutyCycle();


	private:
		Measurement meas;

		//Define Pins -->Signaltable
		static const int Main_Switch_OFF = 27;
		static const int Main_Switch_MANUAL = 28;
		static const int Main_Switch_REMOTE = 29;
		static const int Measure_ADC = A0;			//ADC PIN for Voltage Measurement
		static const int PWM_OUT = 11;
		static const int PWM_INV = 12;

		//Variables for calculating HV
		const float R1 = 100000000;
		const float R2 = 10000;
		const float ADC_Max_Value = 1023.0;
		const float Vcc = 5.0;

		//Define Pins --> Signaltable
		static const int HV_Module_ON = 37;
		static const int HV_Module_OFF = 36;
		static const int HV_Module_Working = 35;
		static const int PWM_Frequency = A1;
		static const int PWM_DutyCycle = A2;

		bool _flybackInitialized;
		bool _timerInitialized;

		// States
		static SwitchStates lastState;
		static bool lastTimerState;
		static int lastPWMFrequency;
		static int lastPWMDutyCycle;

        /**
         * @brief Function to configure the timer settings
         * 
         */
		void timerConfig();

        /**
         * @brief Function to change the PWM frequency and dutyCycle
         * 
         * @param frequency -> The new PWM frequency
         * @param dutyCycle -> The new PWM dutyCycle
         */
		void setPWMFrequency(uint32_t frequency, int dutyCycle);
	};
}

#endif //FLYBACK_H
