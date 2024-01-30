#include "Time.h"

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


std::string Timestamp::printRaw() const {
	std::ostringstream os;
	os << time;
	return os.str();
}
std::string Timestamp::printTime() const {
	return std::format("{:%H:%M:%OS}", time);
}

std::string Timestamp::printDay() const {
	return std::format("{:%d}", time);
}
std::string Timestamp::printMonth() const {
	return std::format("{:%B}", time);
}
std::string Timestamp::printDate() const {
	return std::format("{:%Od %b %Y}", time);
}

timepoint Timestamp::getRawTime() const {
	return time;
}
ymd	Timestamp::getRawYMD() const {
	std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(time) };
	return ymd;
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
		rawDuration = endTime.getRawTime() - startTime.getRawTime();
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

/*
********** TO BE MOVED ************

std::chrono::system_clock::time_point DateTimeHelper::stringToTimepoint(const std::string& time) {
	std::stringstream ss{ time };
	std::tm _tm{};
	ss >> std::get_time(&_tm, "%Y-%m-%d %H:%M:%OS");
	return std::chrono::system_clock::from_time_t(std::mktime(&_tm));
}





void to_json(nlohmann::json& j, const TimeEntry& t) {
	std::ostringstream s1; s1 << t.getRawStartTime();
	std::ostringstream s2; s2 << t.getRawEndTime();

	j = nlohmann::json{
		
		{"name",	t.getName()},
		{"start",	s1.str()},
		{"end",		s2.str()}
	};

}
void from_json(const nlohmann::json& j, TimeEntry& t) {
	std::string name, startBuf, endBuf;

	j.at("name").get_to		(name);
	j.at("start").get_to	(startBuf);
	j.at("end").get_to		(endBuf);

	t.setName(name);
	t.start	(DateTimeHelper::stringToTimepoint(startBuf));
	t.end	(DateTimeHelper::stringToTimepoint(endBuf));
}
*/