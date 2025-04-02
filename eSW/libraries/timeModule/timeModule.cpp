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

String TimeModuleInternals::formatTimeString(const DateTimeStruct& dt)
{
	String timeStr = "";

	timeStr += String(dt.year) + "-";
	timeStr += (dt.month < 10 ? "0" : "") + String(dt.month) + "-";
	timeStr += (dt.day < 10 ? "0" : "") + String(dt.day) + "T";
	timeStr += (dt.hour < 10 ? "0" : "") + String(dt.hour) + ":";
	timeStr += (dt.minute < 10 ? "0" : "") + String(dt.minute) + ":";
	timeStr += (dt.second < 10 ? "0" : "") + String(dt.second) + "Z";

	return timeStr;
}

void TimeModuleInternals::setSystemTime(const DateTimeStruct& newDt)
{
	dt = newDt;
	startMillis = millis();
}

DateTimeStruct TimeModuleInternals::getSystemTime()
{
	updateSoftwareClock();
	return dt;
}

void TimeModuleInternals::updateSoftwareClock()
{
    unsigned long currentMillis = millis();
    unsigned long elapsedMillis = currentMillis - startMillis;

    unsigned long elapsedSeconds = elapsedMillis / 1000;
    startMillis += elapsedSeconds * 1000; // Retain leftover milliseconds for next update

    while (elapsedSeconds > 0)
    {
        incrementTime(&dt);
        elapsedSeconds--;
    }
}

bool TimeModuleInternals::setTimeFromHas(const String& timeString)
{
	int startIdx = timeString.indexOf("\"time\": \"");
	if (startIdx == -1) return false;
	startIdx += 9;

	int endIdx = timeString.indexOf("\"", startIdx);
	if (endIdx == -1) return false;

	String extTime = timeString.substring(startIdx, endIdx);

    if (extTime.length() != 20) return false;

    if (extTime.charAt(4) != '-' || extTime.charAt(7) != '-' ||
    	extTime.charAt(10) != 'T' || extTime.charAt(13) != ':' ||
		extTime.charAt(16) != ':' || extTime.charAt(19) != 'Z')
    {
        return false;
    }

    DateTimeStruct newDt;
    newDt.year   = extTime.substring(0, 4).toInt();
    newDt.month  = extTime.substring(5, 7).toInt();
    newDt.day    = extTime.substring(8, 10).toInt();
    newDt.hour   = extTime.substring(11, 13).toInt();
    newDt.minute = extTime.substring(14, 16).toInt();
    newDt.second = extTime.substring(17, 19).toInt();

    if (newDt.month < 1 || newDt.month > 12 || newDt.day < 1 || newDt.day > 31 ||
        newDt.hour < 0 || newDt.hour > 23 || newDt.minute < 0 || newDt.minute > 59 ||
        newDt.second < 0 || newDt.second > 59)
    {
        return false;
    }

    setSystemTime(newDt);
    return true;
}

TimeModuleInternals* TimeModuleInternals::getInstance()
{
	static TimeModuleInternals instance;
	return &instance;
}

