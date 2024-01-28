#include "Application.h"

Application& Application::getInstance() {
	static Application app;
	return app;
}
Application::Application() {

}
Application::~Application() {

}

bool Application::init() {
	if (createDirectories() && readProjectFolder()) {
		return true;
	}
	else
		return false;

	return false;
}

std::ostringstream	Application::printAllProjectNames()	const {
	std::ostringstream os;

	os << "Projects found: " << m_projectList.size() << '\n';
	for (const auto& p : m_projectList) {
		os << '\t' << p->getName() << '\n';
	}

	return os;
}

bool Application::run() {

	bool success{ false };
	if (!m_init) {
		return false;
	}
	else {
		bool exit{ false };
		while (!exit) {
			
			m_display.home();
		}
	}

	return success;
}
bool Application::saveProject(const Project* project) {
	bool success{ false };

	for (auto& p : m_projectList) {

		// Save if the addresses are the same
		if (p.get() == project) {
			std::string file{ m_projectFolder.string() + p->getName() + ".json" };

			Timestamp ts{};
			ts.stamp();
			p->setLastUpdated(ts);

			if (FileSystem::createFile(file)) {
				std::ofstream output{ file };

				nlohmann::json j{ nlohmann::json::object() };
				j = *p;

				output << std::setw(1) << std::setfill('\t') << j;
				output.close();
			}
		}
	}

	return success;
}


bool Application::createDirectories() {
	try {
		m_parentFolder = ApplicationHelper::getParentDirectory();
	}
	catch(const std::exception& e){
		return false;
	}

	// Set paths
	m_projectFolder = m_parentFolder.string() + PROJECT_FOLDER_STR;
	m_logFolder		= m_parentFolder.string() + LOG_FOLDER_STR;

	if (FileSystem::createDirectory(printProjectFolder())) {
		if (FileSystem::createDirectory(printLogFolder())) {
			m_init = true;
			return true;
		}
	}
	else
		return false;

	return false;
}

bool Application::readProjectFolder() {
	bool success{ false };

	std::vector<std::string> projectNames;
	FileSystem::filesInDirectory(printProjectFolder(), projectNames); // Writes the names found of existing projects to then have them be populated

	if (projectNames.empty()) {
		success = true;
	}
	else {
		for (const std::string& s : projectNames) {

			if(s.find(".json") != std::string::npos ){
				std::ifstream inputFile{ std::filesystem::path{ s }, std::ios::in };

				try {
					nlohmann::json j{ nlohmann::json::parse(inputFile) };

					std::unique_ptr<Project> p{ new Project{ j.template get<Project>() } };
					m_projectList.push_back(std::move(p));
				}
				catch (...) {
					return false;
				}
			}
		}
	}
	return success;
}






std::string ApplicationHelper::getParentDirectory() {

	std::filesystem::path pathBuffer{};

	#ifdef _WIN32
	
		#ifdef _DEBUG
			pathBuffer = "C:\\dev\\1. Project Development Files\\Time-Tracker\\";
			return pathBuffer.string();	
		#endif

	#ifndef _DEBUG
			char buffer[MAX_PATH];
			GetModuleFileNameA(NULL, buffer, MAX_PATH);			// Windows specific call to return parent directory of the EXE
			pathBuffer = buffer;								// Gets the parent directory path
			return pathBuffer.parent_path().string() + "\\";	// return the directory name followed by a backslash
	/*
	*  This is for when I get my developer license and can write to Program Files
	* 
		// If the OS is x64 bit windows, set to normal program files
		#ifdef _WIN64
			pathBuffer 	= "C:\\Program Files\\Time-Tracker\\";			
		#endif

		// If the OS is x32 bit windows, set to x86 program files
		#ifndef _WIN64
			pathBuffer = "C:\\Program Files (x86)\\Time-Tracker\\";
		#endif
	*/
	#endif

#endif


	#ifndef _WIN32
	std::cout << "Windows OS was not detected. This version does not support other operating systems. Exiting Application." << std::endl;
	throw (std::domain_error{"This OS is not supported."});
	#endif

	return pathBuffer.string();
}