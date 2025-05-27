/**
 * @file flyback.h
 * @author Domin
 * @brief Header for the flyback class.
 * @version 0.2
 * @date 2025-05-18
 *
 * @copyright Copyright (c) 2025
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
         * This method sets up the pins and prepares the system for operation.
         */
		void initialize();

		/**
		 * @brief denitialize the Flyback System
		 * This method shuts down the pins and prepares graceful restart.
		 */
		void deinitialize();
        
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

		// TODO MERGE STUFF FROM FRADOM01 into here!
		void setExternPsu(bool state);

		// TODO MERGE STUFF FROM FRADOM01 into here!
		bool getExternPsu();

		/**
		 * @brief Function to regulate voltage to prevent swinging using a hyseresis.
		 * @param targetVoltage -> The requested voltage.
		 * @param hysteresis -> The hysteresis we create to prevent swinging in the system.
		 */
		void regulateVoltage(float targetVoltage, float hysteresis);

		/**
		 * @brief Setter Function for the HAS to regulate the target Voltage.
		 * @param voltage -> The desired Voltage to reach.
		 */
		void setTargetVoltage(float voltage);

		/**
		 * @brief Getter Function for the HAS to know what the current targetVoltage is.
		 * @return The Voltage the system is trying to reach.
		 */
		float getTargetVoltage() const;


		/**
		 * @brief Setter Function for the HAS to regulate the hysteresis.
		 * @param hysteresis -> The desired hysteresis.
		 */
		void setHysteresis(float hysteresis);

		/**
		 * @brief Getter Function for the HAS to know what the current hysteresisVoltage is.
		 * @return The Voltage the system is using to prevent swinging.
		 */
		float getHysteresis() const;


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
		static const int HV_Module_ON = 37;			// Input
		static const int HV_Module_Working = 35;	// Output
		static const int PWM_Frequency = A1;
		static const int PWM_DutyCycle = A2;

		// PSU - Power Supply Unit
		static const int PSU = 36;		// Output

		bool _flybackInitialized;
		bool _timerInitialized;

		// States
		static SwitchStates lastState;
		static bool lastTimerState;
		static int lastPWMFrequency;
		static int lastPWMDutyCycle;

		// Default targetVoltage and hysteresisVoltage for to prevent swinging
		float _targetVoltage = 0.0f;
		float _hysteresis = 0.0f;

		// Rate limiting
		unsigned long _lastRegulationTime = 0;
		const unsigned long _regulationInterval = 100;

		// PID vars
		float _integral = 0.0f;
		float _lastError = 0.0f;
		const float _Kp = 1.5f;
		const float _Ki = 0.05f;
		const float _Kd = 0.1f;

		float _integralMin = -10.0f;
		float _integralMax = 10.0f;

		// Soft start
		bool _softStartActive = true;
		int _softStartDuty = 1;

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
