#include "Framework.h"
#include "Config.h"

#include <iostream>

int main(int argc, char **argv) {	

	// Main() will work as my assembler because I do not wish
	// to create an entire new object and class to do what can be done.

	// Handle arguments
	if(argc > 1){
		std::vector<std::string> args(argv, argv + argc);
		// Display version information
		if(args.at(1) == "--version"){
			std::cout << "\tTime Tracker by Bradley Ryan\n\tVersion " <<
			TimeTracker_VERSION_MAJOR << "." <<
			TimeTracker_VERSION_MINOR << "." <<
			TimeTracker_VERSION_PATCH << std::endl;
			return 0;
		}
		else{
			std::cout << "\tInvalid argument." << std::endl;
			return -1;
		}
	}

	#ifdef __linux__
		std::string projDir {"/mnt/NAS/1-Project-Related/Project-Source-Directories/Time-Tracker/Test-Directory/"};
		std::string settingsPath {projDir + ".timetrackerrc"};
		//std::string home {getenv("HOME")};
		//std::string settingsPath { home + "/.timetrackerrc"};
		//std::string projDir {home + "/Documents/Time-Tracker/"};
	#endif

	#ifdef _Win32
		std::string projDir { /*INPUT DESIRED SAVE DIRECTORY HERE*/};
		std::string settingsPath { /*INPUT DESIRED SETTINGS PATH HERE*/};
	#endif

	try{
		std::shared_ptr<JsonFormat> 			jsonFormat 		{ new JsonFormat };
		std::shared_ptr<ProjectManager> 		projectManager 	{ new ProjectManager };
		std::shared_ptr<FileIOManager> 			fileManager 	{ new FileIOManager{jsonFormat} };

		Framework handler {projectManager, fileManager, settingsPath, projDir};

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