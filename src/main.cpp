#include "Application.h"
#include "Project.h"
#include "json.h"
#include "FileSystem.h"

#include <iostream>

int main(int argc, char* argv[]) {


	std::filesystem::path path{ "C:\\dev\\1. Project Development Files\\Time-Tracker\\testJson.json" };

	Application& app = Application::getInstance();
	app.init();

	app.run();


	/*
	try
	{
		std::ifstream file{ path, std::ios::in };
		nlohmann::json j{ nlohmann::json::parse(file) };
		
		std::cout << j.dump(4) << std::endl;
		auto p = j.template get<Project>();

		std::cout << "\n\n\nNEW:\n";
		std::cout << p.getName() << std::endl;
		std::cout << p.printAllEntries().str() << std::endl;
		std::cout << p.printTotalTime() << std::endl;
	}
	catch (nlohmann::json::parse_error& ex)
	{
		std::cerr << "parse error at byte " << ex.byte << '\n'
			<< ex.what() << '\n'
			<< ex.id << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	*/

	/*
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

	nlohmann::json j{ nlohmann::json::object() };
	j = *proj;

	Application::getInstance().m_projectList.push_back(std::move(proj));
	
	Application::getInstance().saveProjects();
	*/

	/*
	Calendar cal{};

	std::cout << cal.viewWeek().str() << "\n";
	*/

	/*
	Application::getInstance().init();
	std::cout << Application::getInstance().printParentDirectory() << std::endl; 
	std::cout << Application::getInstance().printProjectFolder() << std::endl;

	std::cout << Application::getInstance().printAllProjectNames().str() << std::endl;
	*/

	/* WRITING TO JSON
	TimeEntry t;
	t.start();

	std::string input;
	std::cin >> input;
	
	t.end();

	nlohmann::json j{ nlohmann::json::object()};
	j = t;

	std::ofstream out{ path.string() };
	out << std::setw(1) << std::setfill('\t') << j;
	out.close();
	*/

	/* READ JSON
	TimeEntry TE;
	try
	{
		std::ifstream file{ path, std::ios::in };		
		nlohmann::json two{ nlohmann::json::parse(file) };
		TE = two.template get<TimeEntry>();
	}
	catch (nlohmann::json::parse_error& ex)
	{
		std::cerr << "parse error at byte " << ex.byte << '\n'
			<< ex.what() << '\n'
			<< ex.id << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	*/ 

	
	return 0;
}