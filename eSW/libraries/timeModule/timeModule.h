#ifndef TIMEMODULE_H
#define TIMEMODULE_H

#include <Arduino.h>
#include <String>

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
    	String formatTimeString(const DateTimeStruct &dt);
    	bool setTimeFromHas(const String& timeString);
    	void setSystemTime(const DateTimeStruct& dt);
    	void updateSoftwareClock();
    	DateTimeStruct getSystemTime();

    private:
    	DateTimeStruct dt;
    	unsigned long startMillis = 0;
    };
}

#endif // TIMEMODULE
