#ifndef TIMEMODULE_H
#define TIMEMODULE_H

#include <Arduino.h>

/// @brief namespace for the timeModule \namespace timeModule
namespace timeModule
{
	/// @brief Struct to hold the date and time \struct DateTimeStruct
	typedef struct DateTimeStruct
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
	} DateTimeStruct;

	/// @brief Class to handle Systemtime \class TimeModuleInternals
    class TimeModuleInternals
    {
    public:
    	TimeModuleInternals();
    	~TimeModuleInternals();

		/**
		 * @brief Function to increment the time of the system.
		 *
		 * @param dt -> DateTimeStruct to increment time
		 */
    	static void incrementTime(DateTimeStruct *dt);

		/**
		 * @brief Function to format the time to a string.
		 *
		 * @param dt -> DateTimeStruct to format
		 * @return String -> The formatted time.
		 */
    	static String formatTimeString(const DateTimeStruct &dt);

		/**
		 * @brief Set the Time From Has object to the system time.
		 *
		 * @param timeString -> The time string to set the system time to.
		 * @return true -> if the time was set successfully
		 * @return false -> if the time was not set successfully
		 */
    	bool setTimeFromHas(const String& timeString);

		/**
		 * @brief Set the System Time object of the system.
		 *
		 * @param dt -> DateTimeStruct to set the system time to.
		 */
    	void setSystemTime(const DateTimeStruct& dt);

		/**
		 * @brief Updates the software clock.
		 *
		 */
    	void updateSoftwareClock();

		/**
		 * @brief Get the System Time object
		 *
		 * @return DateTimeStruct -> The system time.
		 */
    	DateTimeStruct getSystemTime();

		/**
		 * @brief Get the Instance object, Singleton pattern.
		 *
		 * @return TimeModuleInternals* -> The instance of the TimeModuleInternals.
		 */
    	static TimeModuleInternals* getInstance();

    private:
    	DateTimeStruct dt;
    	unsigned long startMillis = 0;
    };
}

#endif // TIMEMODULE
