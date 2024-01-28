#include "Project.h"


Project::~Project() {
	delete m_currentTimer;
	for (auto& timeEntry : m_timeEntries) {
		timeEntry.release();
	}
	m_timeEntries.clear();
}
Project& Project::operator=(Project&& old) noexcept {
	if (this != &old) {
		for (auto& timeEntry : old.m_timeEntries) {
			m_timeEntries.push_back(std::move(timeEntry));
		}
		delete m_currentTimer;

		m_name = old.m_name;
		m_currentTimer = old.m_currentTimer;

		old.m_name = "";
		old.m_currentTimer = nullptr;
		old.m_timeEntries.clear();

		return *this;
	}
}
Project::Project(Project&& other) : m_name{ "" } {
	m_name = other.m_name;
	m_currentTimer = other.m_currentTimer;
	for (auto& timeEntry : m_timeEntries) {
		other.m_timeEntries.push_back(std::move(timeEntry));
		timeEntry.release();
	}
	other.m_name = "";
	other.m_currentTimer = nullptr;
	other.m_timeEntries.clear();
}


bool Project::setLastUpdated(Timestamp& timestamp) {
	if (timestamp.isStamped()) {
		m_lastUpdated = timestamp;
		return true;
	}
	else
		return false;
}


bool Project::addTimer(TimeEntry& t) {
	// If the timer is running, add it to the list of timers
	if (t.isRunning()) 
	{
		if (!m_currentTimer) {
			m_timeEntries.push_back(std::make_unique<TimeEntry>(t));

			m_currentTimer = m_timeEntries.back().get();
			return true;
		}
		else
			return false;
		
	}
	else if (!t.isFinished() && !t.isRunning()) {
		return false;
	}
	else if (t.isFinished()) {
		m_timeEntries.push_back(std::make_unique<TimeEntry>(t));

		// SORTING
		//if (m_timeEntries.size() == 1 || t.getRawStartTime() > m_timeEntries[m_timeEntries.size() - 2].getRawStartTime()) {} // Does nothing if no need to sort
		//else {

		//	// Sort the m_timeEntries by start time
		//	for (int i{ 0 }; i < m_timeEntries.size() - 1; ++i) {
		//		for (int x{ i + 1 }; x < m_timeEntries.size(); ++x) {
		//			if (m_timeEntries[x].getRawStartTime() < m_timeEntries[i].getRawStartTime()) {
		//				TimeEntry t1{ m_timeEntries[x] };
		//				TimeEntry t2{ m_timeEntries[i] };

		//				m_timeEntries.em;
		//			}
		//		}
		//	}
		//}

		return true;
	}
	else
		return false;
}

bool Project::startTimer(std::string name) {

	// Create a new timer using name parameter and pass it to member variable
	if (!m_currentTimer) {
		auto t = std::make_unique<TimeEntry>(name);
		t->start();

		addTimer(*t);
		return true;
	}
	else
	{
		return false;
	}

	return false;
}
bool Project::endTimer() {
	bool returnVal{ false };

	// If there is no timer tracked, return false
	if (!m_currentTimer) {
		returnVal = false;
	}
	else {
		// If a timer has been found, and it was successfully stopped, add to list of m_TimeEntries and reset tracker
		if (m_currentTimer->end()) {
			m_currentTimer = nullptr;
			returnVal = true;
		}
		else {
			// If the timer could not be stopped, return false;
			returnVal = false;
		}
	}

	return returnVal;
}

std::ostringstream Project::printAllEntries() const {
	std::ostringstream os;

	if (m_timeEntries.empty()) {
		os << "No Entries for this project.";
	}
	else {
		os << " Date \t\tName\t\tTime Started\tTime Ended\tDuration \n";
		os << "----------------------------------------------------------------\n";
		for (const auto& t : m_timeEntries) {
			os << t->printDate() << "\t" << t->getName() << "\t" << t->printStartTime() << "\t" << t->printEndTime() << "\t" << t->printDuration() << "\n";
		}
	}
	
	return os;
}

std::string	Project::printTotalTime() const {
	std::chrono::system_clock::duration totalTime{};


	if (m_timeEntries.size() < 1) {
		return "00:00:00";
	}
	else if (m_timeEntries.size() == 1) {
		return m_timeEntries[0]->printDuration();
	}
	else {

		for (const auto& t : m_timeEntries) {
			totalTime = totalTime + t->getRawDuration();
		}
	}

	return std::format("{:%H:%M:%OS}", totalTime);
}



void to_json(nlohmann::json& j, const Project& p) {

	j = nlohmann::json{
		{"name", p.getName()},
		{"updated", p.m_lastUpdated.print()}
	};

	int i{ 0 };
	nlohmann::json timers = nlohmann::json::array();

	for (const auto& t : p.m_timeEntries) {
		std::ostringstream s1; s1 << t->getRawStartTime();
		std::ostringstream s2; s2 << t->getRawEndTime();

		if (t->getName() == "") {
			std::ostringstream name;
			name << "Timer " << i;

			timers.push_back({ name.str(), s1.str(), s2.str() } );
		}
		else
			timers.push_back({ t->getName(), s1.str(), s2.str()});

		++i;
	}

	j["timers"] = timers;
}

void from_json(const nlohmann::json& j, Project& p) {
	std::string nameBuf, lastUpdatedBuf;
	
	j.at("name").get_to(nameBuf);
	j.at("updated").get_to(lastUpdatedBuf);

	Timestamp timestamp{ DateTimeHelper::stringToTimepoint(lastUpdatedBuf) };
	p.setLastUpdated(timestamp);

	p.setName(nameBuf);

	nlohmann::json t;
	j.at("timers").get_to(t);
	
	int i{ 0 };
	for (auto& elm : t.items()) {
		nlohmann::json obj = elm.value();
		std::string name, start, end;

		obj.at(0).get_to(name);
		obj.at(1).get_to(start);
		obj.at(2).get_to(end);

		TimeEntry t{ name };

		t.start(DateTimeHelper::stringToTimepoint(start));
		t.end(DateTimeHelper::stringToTimepoint(end));

		p.addTimer(t);
	}
}


bool ProjectHelper::createProjectFromFile(const std::string& file, Project& p) {
	std::ifstream inputFile{ std::filesystem::path{ file }, std::ios::in };

	try {
		nlohmann::json j{ nlohmann::json::parse(inputFile) };

		p = j.template get<Project>();

		return true;
	}
	catch (...) {
		return false;
	}

	return false;
}