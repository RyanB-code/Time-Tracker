#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <format>

#include <json.h>


using timepoint = std::chrono::system_clock::time_point;
using sysclock 	= std::chrono::system_clock;
using ymd		= std::chrono::year_month_day;

class Timestamp {
public:
	Timestamp();
	Timestamp(const timepoint& time);
	virtual ~Timestamp();

	virtual bool stamp(const timepoint& time = sysclock::now());
	
	bool IsStamped() 			const;

	std::string printRaw()		const;
	std::string printTime()		const;		// Prints the HH:MM::SS			(14:23:01)

	std::string printDay()		const;
	std::string printMonth()	const;
	std::string printDate()		const;		// Prints in DD MMM YYYY format (12 Dec 2022)

	timepoint	getRawTime()	const;
	ymd			getRawYMD()		const;

private:
	timepoint time{};
	bool m_isStamped{ false };
};



class Timer 
{
public:
	Timer();
	Timer(const timepoint& start);
	virtual ~Timer();

	bool start	(const timepoint& time = sysclock::now());
	bool end	(const timepoint& time = sysclock::now());

	bool IsRunning()	const;
	bool IsFinished()	const;

	sysclock::duration	getRawDuration()	const;	
	timepoint			getRawStartTime()	const;
	timepoint			getRawEndTime()		const;

	std::string printDate()			const;
	std::string printDuration()		const;
	std::string printStartTime()	const;
	std::string printEndTime()		const;

private:
	Timestamp	startTime{}, endTime{};
	sysclock::duration		rawDuration{};

	bool isRunning	{ false };
	bool isFinished	{ false };
};



#endif