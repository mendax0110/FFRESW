#ifndef TIMEMODULE_H
#define TIMEMODULE_H

#include <Arduino.h>

namespace timeModule
{
	typedef struct DateTimeStruct
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
	} DateTimeStruct;

    class TimeModuleInternals
    {
    public:
    	TimeModuleInternals();
    	~TimeModuleInternals();

    	void incrementTime(DateTimeStruct *dt);

    };
}

#endif // TIMEMODULE
