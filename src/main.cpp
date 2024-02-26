#include "Framework.h"

#include <iostream>

int main(int argc, char* argv[]) {	

	// Main will work as my assembler because I do not wish
	// to create an entire new object and class to do what can be done.

	#ifdef __linux__
		std::string projectPath {"/usr/local/share/projects/Time-Tracker/Test-Directory/Projects"};
		// std::string projectPath {"/usr/local/bin/Time-Tracker/}; // For realease/deployment
		std::string settingsFile {"/usr/local/share/projects/Time-Tracker/Test-Directory/Settings.json"};
	#endif

	#ifdef _Win32
		std::string projectPath { /*INPUT DESIRED SAVE DIRECTORY HERE*/};
		std::string settingsFile { /*INPUT DESIRED SETTINGS PATH HERE*/};
	#endif

	try{
		std::shared_ptr<JsonFormat> 	jsonFormat 		{ new JsonFormat };
		std::shared_ptr<ProjectManager> projectManager 	{ new ProjectManager };
		std::shared_ptr<FileIOManager> 	fileManager 	{ new FileIOManager{ jsonFormat} };
		fileManager->setDirectory(projectPath);

		std::shared_ptr<Settings> 		settings		{ new Settings{projectPath, settingsFile}};

		Framework handler (projectManager, fileManager, settings);

		if(handler.setup()){
			handler.run();
		}
		else
			std::cout << "Error in setup. Application exited.\n";
	}
	catch(std::exception& e){
		std::cout << "\t" << e.what() << "\n\tApplication aborted.\n";
	}	
	return 0;
}