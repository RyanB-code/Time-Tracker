#include "Timing.h"

Timestamp::Timestamp(){
	
}
Timestamp::Timestamp(const timepoint& timestamp) {
	stamp(timestamp);
}
Timestamp::~Timestamp(){

}

bool Timestamp::stamp(const timepoint& timepoint) {	
	if (!m_isStamped) {
		time = timepoint;
		m_isStamped = true;
		return true;
	}
	else
		return false;
}
bool Timestamp::IsStamped() const{
	return m_isStamped;
}



std::string Timestamp::printTime() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time }; // Makes zoned time

	// Needed to output the seconds as 2 digits instead of a float
	std::string seconds {std::format("{:%S}", zonedTime)};
	std::ostringstream formattedTime;
	formattedTime << std::format("{:%H:%M:}", zonedTime) << seconds.substr(0,2);

    return formattedTime.str();
}

std::string Timestamp::printDay() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%d}", zonedTime);
}
std::string Timestamp::printMonth() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%B}", zonedTime);
}
std::string Timestamp::printDate() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%Od %b %Y}", zonedTime);
}
timepoint Timestamp::getRawTime() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return zonedTime;
}


Timer::Timer(){

}
Timer::Timer(const timepoint& start) {
	this->start(start);
}
Timer::~Timer(){

}

bool Timer::start(const timepoint& time) {
	if (!isFinished && !isRunning) {
		startTime.stamp(time);
		isRunning = true;
		return true;
	}
	else {
		return false;
	}

	return false;
}
bool Timer::end(const timepoint& end) {

	if (isRunning && !isFinished) {
		endTime.stamp(end);
		rawDuration = (std::chrono::time_point<std::chrono::system_clock>)endTime.getRawTime() - (std::chrono::time_point<std::chrono::system_clock>)startTime.getRawTime();
		isFinished = true;
		isRunning = false;
		return true;
	}
	else
		return false;

	return false;
}

bool Timer::IsRunning() const{
	return isRunning;
}
bool Timer::IsFinished() const {
	return isFinished;
}

sysclock::duration	Timer::getRawDuration()	const { 
	return rawDuration; 
}
timepoint		Timer::getRawStartTime()	const {
	 return startTime.getRawTime(); 
}
timepoint		Timer::getRawEndTime()		const { 
	return endTime.getRawTime(); 
}

std::string Timer::printDate()	const {
	 return startTime.printDate(); 
}

std::string Timer::printDuration() const {
	return std::format("{:%H:%M:%S}", rawDuration);
}
std::string Timer::printStartTime() const {
	return std::format("{:%H:%M:%S}", this->getRawStartTime());
}
std::string Timer::printEndTime() const {
	return std::format("{:%H:%M:%S}", this->getRawEndTime());
}
