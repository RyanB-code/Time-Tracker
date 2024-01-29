#ifndef PROJECT_H
#define PROJECT_H

#include "DateTime.h"
#include "json.h"

#include <algorithm>
#include <sstream>
#include <fstream>

class Project
{
public:
	Project(std::string name = "New Project") : m_name{ name } { }
	~Project();

	Project(Project&& other);
	Project& operator=(Project&& p) noexcept;


	bool setLastUpdated(Timestamp& timestamp);

	void		setName(std::string name)			{ m_name = name; }
	std::string getName()					const	{ return m_name; }

	bool addTimer(TimeEntry& timer);
	bool startTimer(std::string name="Unnamed Timer");
	bool endTimer();

	friend void to_json(nlohmann::json& j, const Project& p);
	friend void from_json(const nlohmann::json& j, Project& p);

	std::ostringstream	printAllEntries() const;
	std::string			printTotalTime() const;

private:
	std::string m_name;

	TimeEntry* m_currentTimer{nullptr};
	Timestamp m_lastUpdated{};

	std::vector<std::unique_ptr<TimeEntry>> m_timeEntries{};

};


// Helper Functions
namespace ProjectHelper {
	bool createProjectFromFile(const std::string& file, Project& p);
}


#endif