#include "Framework.h"

#include <iostream>

int main(int argc, char* argv[]) {	

	// Main() will work as my assembler because I do not wish
	// to create an entire new object and class to do what can be done.

	#ifdef __linux__
		std::string homeDirectory {"/usr/local/share/projects/Time-Tracker/Test-Directory/"};
		std::string projDirectory {homeDirectory + "Projects/"};
	#endif

	#ifdef _Win32
		std::string projectPath { /*INPUT DESIRED SAVE DIRECTORY HERE*/};
		std::string settingsFile { /*INPUT DESIRED SETTINGS PATH HERE*/};
	#endif

	try{
		std::shared_ptr<JsonFormat> 			jsonFormat 		{ new JsonFormat };
		std::shared_ptr<SettingsJsonFormat>		settingsJson	{ new SettingsJsonFormat };
		std::shared_ptr<ProjectManager> 		projectManager 	{ new ProjectManager };
		std::shared_ptr<FileIOManager> 			fileManager 	{ new FileIOManager{ jsonFormat, settingsJson} };

		fileManager->setProjectDirectory(projDirectory);
		fileManager->setHomeDirectory(homeDirectory);

		Framework handler {projectManager, fileManager};

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