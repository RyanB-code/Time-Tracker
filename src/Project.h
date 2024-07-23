#pragma once

#include "Timing.h"
#include "json.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>

namespace TimeTracker{

	namespace ProjectHelper{
		constexpr std::string UNNAMED_ENTRY { "Unnamed Entry"};
	}
	class ProjectEntry : public Timer {
	public:
		ProjectEntry(const std::string& s=ProjectHelper::UNNAMED_ENTRY);
		virtual ~ProjectEntry();

		std::string_view 	getName() 	const;
		int					getID() 	const;

		void 				setName(const std::string&);
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

		void setName(const std::string& s);

		std::string_view 				getName() 					const;
		Timestamp 						getLastUpdated() 			const;
		const std::vector<EntryPtr>& 	getEntries() 				const;
		std::shared_ptr<Timestamp> 		getRunningTimerStartTime() 	const;

		bool startTimer(std::string name=ProjectHelper::UNNAMED_ENTRY);
		bool endTimer();

		bool isTimerRunning() const;

		bool addEntry(EntryPtr entry);
		bool removeEntry(int id);

		std::ostringstream	printAllEntries(uint8_t entryNameWidth=20) const;
		std::string			printTotalTime() const;

	private:
		std::string	 			name		{};
		Timestamp 				lastUpdated	{};
		std::vector<EntryPtr>	entries		{};
		EntryPtr 				runningEntry{};

		bool setLastUpdated(Timestamp& timestamp);
	};

	using ProjectPtr = std::shared_ptr<Project>;

	class ProjectManager{
	public:
		ProjectManager();
		~ProjectManager();

		ProjectPtr 									getProject()			const;
		std::vector<std::string> 					getAllProjectNames() 	const;
		std::vector<std::shared_ptr<const Project>> getAllProjects() 		const;

		ProjectPtr findProject(std::string name) const;

		bool selectProject(std::string name);
		void deselectProject();

		bool addProject(ProjectPtr project);
		bool deleteProject(std::string name);
	private:
		std::map<std::string, ProjectPtr> 	projects;
		std::weak_ptr<Project> 				selectedProject;
	};

}