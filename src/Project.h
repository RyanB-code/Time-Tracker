#ifndef PROJECT_H
#define PROJECT_H

#include "Time.h"
#include "json.h"

#include <algorithm>
#include <sstream>
#include <fstream>

class ProjectEntry : public Timer {
public:
	ProjectEntry(const std::string& s = "New Timer");
	virtual ~ProjectEntry();

	std::string_view 	getName() const;
	void 				setName(const std::string& s);

	int					getID() const;

private:
	std::string name{};
	int id{};

	int setID();
};

using EntryPtr = std::shared_ptr<ProjectEntry>;



class Project
{
public:
	Project(const std::string& s = "New Project");
	~Project();

	void				setName(const std::string& s);
	std::string_view 	getName() const;

	Timestamp 	getLastUpdated() const;

	bool addEntry(EntryPtr entry);
	bool removeEntry(int id);
	const std::vector<EntryPtr>& getEntries() const;

	bool startTimer(std::string name="Unnamed Entry");
	bool endTimer();

	std::ostringstream	printAllEntries() const;
	std::string			printTotalTime() const;

private:
	std::string name{};

	Timestamp lastUpdated{};

	std::vector<EntryPtr> entries{};
	EntryPtr runningEntry{};

	bool setLastUpdated(Timestamp& timestamp);

};



#endif

/*
****** MAY NEED FOR FILE I/O *****************
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
*/