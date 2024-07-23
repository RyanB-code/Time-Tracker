#include "Framework.h"

#include <iostream>

int main(int argc, char **argv) {	

	using namespace TimeTracker;

	// Main() will work as my assembler because I do not wish
	// to create an entire new object and class to do what can be done.

	// Handle arguments
	if(argc > 1){
		std::vector<std::string> args(argv, argv + argc);
		// Display version information
		if(args.at(1) == "--version"){
			std::cout << getVersion();
			return 0;
		}
		else{
			std::cout << "\tInvalid argument." << std::endl;
			return -1;
		}
	}

	std::string rcPath 	{determineRCPath()};
	std::string saveDir	{determineSaveDirectory()};

	try{
		std::shared_ptr<JsonFormat> 			jsonFormat 		{ new JsonFormat };
		std::shared_ptr<ProjectManager> 		projectManager 	{ new ProjectManager };
		std::shared_ptr<FileIOManager> 			fileManager 	{ new FileIOManager{jsonFormat} };

		TimeTracker::Framework handler {projectManager, fileManager, rcPath, saveDir};

		if(handler.setup()){
			handler.run();
		}
		else
			std::cerr << "Error in setup. Application exited.\n";
	}
	catch(std::exception& e){
		std::cerr << "\t" << e.what() << "\n\tApplication aborted.\n";
	}	
	return 0;
}