/**
* @file vacControl.h
* @author Domin
* @brief Header for the vacControl class.
* @version 0.2
* @date 2025-05-18
*
* @copyright Copyright (c) 2025
*/
#ifndef VACCONTROL_H
#define VACCONTROL_H

#include <Arduino.h>
#include <Wire.h>


/// @brief Namespace for the VacControl module \namespace vacControlModule
namespace vacControlModule
{
	/// @brief Enum to represent the states of the main switch and pump \enum MainSwitchStates
	enum class MainSwitchStates : int
	{
		Main_Switch_OFF,
		Main_Switch_MANUAL,
		Main_Switch_REMOTE,
		Main_switch_INVALID
	};

	/// @brief Enum for extern Setup for pinMode for the PumpState \enum PumpState
	enum class PumpState : int
	{
		pump_OFF,
		pump_ON
	};

	/// @brief Enum to represent the different operating scenarios of the VacControl system
	enum Scenarios
	{
		Scenario_1 = 0,
		Scenario_2 = 1,
		Scenario_3 = 2,
		Scenario_4 = 3,
		Scenario_5 = 4,
		Invalid_Scenario = -1
	};

	/// @brief Structure to store the measured values of the system \struct Measurement
	/// This structure holds the pressure values measured from the system.
	typedef struct Pressure
	{
		float pressure;

	} meas;

	/// @brief VacControl class to manage the vacuum control system
	/// This class provides methods for initializing the system, configuring the timer,
	/// measuring parameters, and handling different system states such as ON, OFF, HAND, and REMOTE modes.
	class VacControl
	{
	public:

		VacControl();
		~VacControl();

		/**
		 * @brief Initialize the VacControl System
		 * This method sets up the pins and prepares the system for operation.
		 */
		void initialize();

		/**
		 * @brief denitialize the VacControl System
		 * This method shuts down the pins and prepares graceful restart.
		 */
		void deinitialize();

		/**
		 * @brief Get the state of the VacControl system
		 *
		 * @return true -> VacControl is initialized and ready
		 * @return false -> VacControl is not initialized
		 */
		bool isInitialized() const;

		/**
		 * @brief Returns the state of the main switch
		 *
		 * @return The current state of the switch (Main_Switch_OFF, Main_Switch_MANUAL, etc.)
		 */
		MainSwitchStates getMainSwitchState();

		/**
		 * @brief Returns the state of the pump
		 *
		 * @return The current state of the output for the pump (pump_ON, pump_OFF)
		 */
		PumpState getPumpState();

		/**
		 * @brief Executes logic depending on which Main-Switch state is active
		 *
		 * This function decides which scenario to run based on the current state of the system.
		 */
		Scenarios getScenario();

		/**
		 * @brief Measures the actual pressure of the system
		 *
		 * @return Measurement -> A Measurement object containing the current pressure
		 */
		Pressure measure();

		/**
		 * @brief Returns the currently selected target pressure
		 * @return float target pressure value
		 */
		float getCurrentTargetPressure();

		/**
		 * @brief Controls the vacuum LED based on the current and target pressures
		 *
		 * @param pressure The current pressure in the system
		 * @param targetPressure The target pressure to reach
		 */
		void setVacuumLed(float pressure, float targetPressure);

		 /**
		  * @brief Determines the scenario based on the potentiometer value
		  *
		  * @param potValue The value read from the potentiometer (used for pressure regulation)
		  * @return The corresponding scenario based on the potentiometer value
		  */
		int getScenarioFromPotValue(int potValue);


		/**
		 * @brief Set the Pump flag
		 *
		 * @param flag This is the boolean flag to set
		 */
		void setPump(bool flag);

		/**
		 * @brief Runs the main control loop for the VacControl system
		 *
		 * This function checks the current system state and performs actions accordingly (e.g., switch states, pump control, LED control).
		 */
		void run();

		/**
		 * @brief Function to set an external scenario, typically from remote input
		 *
		 * @param pressure The external scenario pressure value
		 */
		void setExternScenario(int pressure);

		/**
		 * @brief Getter function to retrieve the current external scenario state
		 *
		 * @return The current external scenario state (integer)
		 */
		int getExternScenario();

		/**
		 * @brief Process external data for scenarios (currently unused)
		 *
		 * This function could be expanded to process external scenario commands if needed.
		 */
		void externPump(int pumpState);

		/**
		 * @brief Function to get the desired PinMode from HAS
		 * @param state -> the state to change the PinMode
		 *
		 */
		void setExternPump(int state);

		/**
		 * @ brief Getter Function to get the PinMode
		 *
		 */
		int getExternPump();

		/**
		 * @brief Sets the external pressure value
		 *
		 * @param pressure The external pressure value to set
		 */
		void setExternPressure(float pressure);

		 /**
		  * @brief Gets the external pressure value
		  *
		  * @return The current external pressure value
		  */
		float getExternPressure();


	private:
		Pressure meas;


		//Define Pins --> Input
		static const int Main_Switch_OFF = 27;		//Main_Switch OFF Mode 27
		static const int Main_Switch_MANUAL = 28;	//Main_Switch Manual Mode 28
		static const int Main_Switch_REMOTE = 29;	//Main_Switch Remote Mode 29
		static const int Switch_Pump_ON = 23;		//Button to turn Pump ON 23
		static const int targetPressure = A4;		//Potentiometer for Regulation

		//Define Pins --> Output
		static const int Pump_Status_LED = 24; 		//OUTPUT to see State off Pump
		static const int Pump_Relay = 25;			//OUTPUT to turn on/off Relais
		static const int targetVacuumLED = 26; 		//OUTPUT to see Vacuum reached

		//Variables to save Values
		int currentScenario = -1;
		int currentPumpState = -1;

		//Variables for TargetPressure
		static const float TARGET_PRESSURE_1 = 0.1f; 	//0.1mbar
		static const float TARGET_PRESSURE_2 = 0.05f;	//0.05mbar
		static const float TARGET_PRESSURE_3 = 0.03f;	//0.03mbar

		//States
		static int lastState;
		static int lastPumpState;

		bool _vacControlInitialized;

		/**
		 * @brief Logic for when switch is OFF
		 */
		void handleOffMode();

		/**
		 * @brief Logic for when switch is in MANUAL mode
		 */
		void handleManualMode();

		/**
		 * @brief Logic for when switch is in REMOTE mode
		 */
		void handleRemoteMode();

		/**
		 * @brief Logic for when switch is Invalid
		 */
		void handleInvalidMode();
	};
}

#endif //VACCONTROL_H
