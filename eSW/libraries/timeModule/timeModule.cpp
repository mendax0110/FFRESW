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


