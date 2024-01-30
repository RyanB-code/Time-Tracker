#include "Project.h"

#include <cstdlib>

#include <iostream>

int main(int argc, char* argv[]) {

	
	std::unique_ptr<Project> proj{ new Project {"newProj3"} };

	proj->startTimer();

	std::string i;
	std::cin >> i;

	proj->endTimer();

	proj->startTimer();
	std::cin >> i;
	proj->endTimer();

	proj->startTimer();
	std::cin >> i;
	proj->endTimer();

	std::cout << proj->printAllEntries().str() << "\n\nTotal: " << proj->printTotalTime() << "\n";


	
	return 0;
}