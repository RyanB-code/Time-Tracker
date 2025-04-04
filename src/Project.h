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
		void					getEntries(std::vector<EntryPtr>& entryList)		const;
		std::shared_ptr<Timestamp> 		getRunningTimerStartTime() 	const;
		uint64_t 						getEntriesSize()			const;

		bool startTimer(std::string name=ProjectHelper::UNNAMED_ENTRY);
		bool startTimer(timepoint startTime, std::string name=ProjectHelper::UNNAMED_ENTRY);
		bool endTimer();

		bool isTimerRunning() const;

		bool addEntry(EntryPtr entry);
		bool removeEntry(int id);

		std::ostringstream	printAllEntries			( uint8_t entryNameWidth=20) 								const;
		std::ostringstream 	printRecentEntries		( uint8_t entryNameWidth=20, uint64_t lastRecentEntries=5)	const;
		std::ostringstream 	printRecentEntryRange	( uint64_t start, uint64_t interval, uint8_t entryNameWidth=20)	const;
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

		ProjectPtr 									getSelectedProject()	const;
		std::vector<std::string> 					getAllProjectNames() 	const;
		std::vector<std::shared_ptr<const Project>> getAllProjects() 		const;

		ProjectPtr findProject(std::string name) const;						// nullptr if doesnt exist

		bool selectProject(std::string name);
		void deselectProject();

		bool addProject(ProjectPtr project);
		bool deleteProject(std::string name);

		void clearProjects();
	private:
		std::map<std::string, ProjectPtr> 	projects;
		std::weak_ptr<Project> 				selectedProject;
	};

	namespace ProjectHelper{
		std::ostringstream printEntryTable(const std::vector<EntryPtr>& entries, uint8_t entryNameWidth=20, std::pair<bool, uint64_t> numberingInfo={false, 0}); // Numbering info - bool is whether or not you want it included, the int is IF TRUE, what number to start at
	}
}
