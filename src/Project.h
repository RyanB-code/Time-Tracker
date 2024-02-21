#ifndef PROJECT_H
#define PROJECT_H

#include "Time.h"
#include "json.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>

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
	bool isTimerRunning() const;

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

using ProjectPtr = std::shared_ptr<Project>;

class ProjectManager{
public:
	ProjectManager();
	~ProjectManager();

	ProjectPtr getProject();
	std::vector<std::string> getAllProjectNames() const;
	ProjectPtr findProject(std::string name) const;

	bool selectProject(std::string name);
	void deselectProject();

	bool addProject(ProjectPtr project);
	bool deleteProject(std::string name);

private:

	std::map<std::string, ProjectPtr> 	projects;
	std::weak_ptr<Project> 				selectedProject;

};

#endif