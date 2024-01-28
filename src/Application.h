#ifndef APPLICATION_H
#define APPLICATION_H


#include <string>
#include <exception>

#include "FileSystem.h"
#include "Project.h"
#include "Display.h"

class Application final {
public:
	static Application& getInstance();
	~Application();

	bool init();
	bool run();

	bool saveProject(const Project* project);

	std::string			printParentDirectory()	const { return m_parentFolder.string();		}
	std::string			printProjectFolder()	const { return m_projectFolder.string();	}
	std::string			printLogFolder()		const { return m_logFolder.string();		}

	std::ostringstream	printAllProjectNames()	const;
	int					printNumProjFound()		const { return m_projectList.size(); }

	std::vector<std::unique_ptr<Project>> m_projectList;


private:
	Application();
	const Application& operator=(const Application& app) = delete;

	bool createDirectories();
	bool readProjectFolder();


	bool m_init{ false };

	Display::DisplayManager& m_display = Display::DisplayManager::getInstance();

	std::filesystem::path m_parentFolder{}, m_projectFolder{}, m_logFolder{};

	const std::string PROJECT_FOLDER_STR{ "Projects\\" }, LOG_FOLDER_STR{ "Logs\\" };
};

namespace ApplicationHelper {
	std::string getParentDirectory();
}

#endif