/**
 * @file timeModule.cpp
 * @brief Implementation of the timeModule class.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 */
#include <timeModule.h>

using namespace timeModule;

TimeModuleInternals::TimeModuleInternals()
{

}

TimeModuleInternals::~TimeModuleInternals()
{

}

/// @brief Increments the given DateTimeStruct object by one second.
/// @param dt Pointer to the DateTimeStruct object to be updated.
void TimeModuleInternals::incrementTime(DateTimeStruct* dt)
{
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    dt->second++;
    if (dt->second >= 60)
    {
        dt->second = 0;
        dt->minute++;
        if (dt->minute >= 60)
        {
            dt->minute = 0;
            dt->hour++;
            if (dt->hour >= 24)
            {
                dt->hour = 0;
                dt->day++;
                // Leap year check for February
                int daysThisMonth = daysInMonth[dt->month - 1];
                if (dt->month == 2 && ((dt->year % 4 == 0 && dt->year % 100 != 0) || (dt->year % 400 == 0)))
                {
                    daysThisMonth = 29;
                }
                if (dt->day > daysThisMonth)
                {
                    dt->day = 1;
                    dt->month++;
                    if (dt->month > 12)
                    {
                        dt->month = 1;
                        dt->year++;
                    }
                }
            }
        }
    }
}

/// @brief Formats a DateTimeStruct into a readable string format.
/// @param dt The DateTimeStruct object containing the date and time.
/// @return A formatted string representation of the date and time.
String TimeModuleInternals::formatTimeString(const DateTimeStruct& dt)
{
	String timeStr = "";

	timeStr += String(dt.year);
	timeStr += "/";
	timeStr += (dt.month < 10) ? "0" + String(dt.month) : String(dt.month);
	timeStr += "/";
	timeStr += (dt.day < 10) ? "0" + String(dt.day) : String(dt.day);
	timeStr += " ";
	timeStr += (dt.hour < 10) ? "0" + String(dt.hour) : String(dt.hour);
	timeStr += ":";
	timeStr += (dt.minute < 10) ? "0" + String(dt.minute) : String(dt.minute);
	timeStr += ":";
	timeStr += (dt.second < 10) ? "0" + String(dt.second) : String(dt.second);

	return timeStr;
}

/// @brief Sets the system time to the provided DateTimeStruct.
/// @param newDt The new DateTimeStruct to set as the system time.
void TimeModuleInternals::setSystemTime(const DateTimeStruct& newDt)
{
	dt = newDt;
	startMillis = millis();
}

/// @brief Retrieves the current system time.
/// @return A DateTimeStruct containing the updated system Time.
DateTimeStruct TimeModuleInternals::getSystemTime()
{
	updateSoftwareClock();
	return dt;
}

/// @brief Updates the software clock by accounting for elapsed time.
void TimeModuleInternals::updateSoftwareClock()
{
	unsigned long elapsedMillis = millis() - startMillis;
	unsigned long elapsedSeconds = elapsedMillis / 1000;

	startMillis = millis();

	while (elapsedSeconds > 0)
	{
		dt.second++;
		incrementTime(&dt);
		elapsedSeconds--;
	}
}

/// @brief Parses a time string and sets the system time if valid.
/// @param timeString The input time string in the format "YYYY/MM/DD HH:MM:SS".
/// @return True if the time was successfully set, false otherwise.
bool TimeModuleInternals::setTimeFromHas(const String& timeString)
{
    if (timeString.length() != 19) return false;

    if (timeString.charAt(4) != '/' || timeString.charAt(7) != '/' ||
        timeString.charAt(10) != ' ' || timeString.charAt(13) != ':' ||
        timeString.charAt(16) != ':')
    {
        return false;
    }

    DateTimeStruct newDt;
    newDt.year   = timeString.substring(0, 4).toInt();
    newDt.month  = timeString.substring(5, 7).toInt();
    newDt.day    = timeString.substring(8, 10).toInt();
    newDt.hour   = timeString.substring(11, 13).toInt();
    newDt.minute = timeString.substring(14, 16).toInt();
    newDt.second = timeString.substring(17, 19).toInt();

    if (newDt.month < 1 || newDt.month > 12 || newDt.day < 1 || newDt.day > 31 ||
        newDt.hour < 0 || newDt.hour > 23 || newDt.minute < 0 || newDt.minute > 59 ||
        newDt.second < 0 || newDt.second > 59)
    {
        return false;
    }

    setSystemTime(newDt);
    return true;
}




