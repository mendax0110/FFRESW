/*
 * flyback.h
 *
 *  Created on: 28.03.2025
 *      Author: domin
 */
#ifndef VACCONTROL_H
#define VACCONTROL_H

#include <Arduino.h>
#include <Wire.h>


/// @brief Namespace for the Flyback module \namespace flybackModule
namespace vacControl
{
	enum class SwitchStates : int
	{
		Main_Switch_OFF,
		Main_Switch_MANUAL,
		Main_Switch_REMOTE,
		Main_switch_INVALID,
		PUMP_ON,
		PUMP_OFF
	};
	/// @brief Structure to store the measured values of the system \struct Measurement
	/// This structure holds the voltage, current, and power values measured from the system.
	typedef struct Measurement
	{
		float voltage;		//measured Voltage
		float current;		//measured Current
		float power;		//measured power
		int digitalValue; 	//measured DigitalValue for Frequency 0--1023
		int frequency;		//current PWM - Frequency
	} meas;

	/// @brief Flyback class to manage the Flyback system \class Flyback
	/// This class provides methods for initializing the system, configuring the timer,
	/// measuring parameters, and handling different system states such as ON, OFF, HAND, and REMOTE modes.
	class vacControl
	{
	public:

		vacControl();
		~vacControl();

        /**
         * @brief Initialize the VacControl System
         *
         */
		void initialize();

        /**
         * @brief Get the state of the VacControl system
         *
         * @return true -> VacControl is initialized
         * @return false -> VacControl is not initialized
         */
		bool isInitialized() const;

        /**
         * @brief Returns the state of the timer
         *
         * @return true -> if the timer is initialized
         * @return false -> if the timer is not initialized
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
		void setExternPressure(int pressure);

		/**
		 * @brief Getter Function to get the Frequency
		 *
		 */
		int getExternPressure();


	private:
		Measurement meas;

		//Define Pins --> Main_Switch
		static const int Main_Switch_OFF = 27;		//Main_Switch OFF Mode 27
		static const int Main_Switch_MANUAL = 28;	//Main_Switch Manual Mode 28
		static const int Main_Switch_REMOTE = 29;	//Main_Switch Remote Mode 29

		//Define Pins --> Vacuum Logic
		static const int Switch_Pump_ON = 37;			//Button to turn Pump ON 37
		static const int Switch_Pump_OFF = 36;			//Button to turn Pump OFF 36
		static const int targetPressure = A2;	//Potentiometer for Regulation

		int externPressure = 0;

		bool _vacControlInitialized;


        /**
         * @brief Prints an error message via the serial connection
         *
         * @param errorMessage -> The error message to be printed
         */
		void reportError(const char* errorMessage);
	};
}

#endif //VACCONTROL_H
