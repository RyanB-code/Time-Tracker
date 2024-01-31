#include "Project.h"
#include "FileIO.h"

#include <iostream>

int main(int argc, char* argv[]) {

	
	Project proj{"Sup Bitches"};

	proj.startTimer();

	std::string i;
	std::cin >> i;

	proj.endTimer();

	proj.startTimer();
	std::cin >> i;
	proj.endTimer();

	proj.startTimer();
	std::cin >> i;
	proj.endTimer();

	std::cout << proj.printAllEntries().str() << "\n\nTotal: " << proj.printTotalTime() << "\n";

	std::cout << proj.getLastUpdated().printTime() << std::endl;
	

	std::string path {"/usr/local/share/projects/Time-Tracker/textfile.txt"};
	JsonFormat jsonWriter{};

	jsonWriter.write(path, proj);

	Project newProj {jsonWriter.read(path)};

	std::cout << "\n\nAfter read:\n\n" << newProj.printAllEntries().str() << "\n\nTotal: " << newProj.printTotalTime() << "\n";

	
	return 0;
}