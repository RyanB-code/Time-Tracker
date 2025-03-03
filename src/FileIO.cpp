#include "FileIO.h"

using json = nlohmann::json;
using namespace TimeTracker;

ProjectFormatter::ProjectFormatter(){

}
ProjectFormatter::~ProjectFormatter(){

}


JsonFormat::JsonFormat(){

}
JsonFormat::~JsonFormat(){

}
bool JsonFormat::write(const std::string& path, const Project& project) const{
	// Write basic information
    json j{
		{"name", 	project.getName()},
		{"updated", project.getLastUpdated().printTime()}
	};

	// Print all entries in the project
	nlohmann::json entries = nlohmann::json::array();

	std::vector<EntryPtr> projEntries;
	project.getEntries(projEntries);

	for (const auto& t : projEntries) {
		std::ostringstream startBuf, endBuf;
		startBuf 	<< t->getRawStartTime();
		endBuf 		<< t->getRawEndTime();

		// If entry wasnt given a name, write by ID
		if (t->getName() == ProjectHelper::UNNAMED_ENTRY) {
			std::ostringstream name;
			name << "Entry " << t->getID();

			entries.push_back({ name.str(), startBuf.str(), endBuf.str() } );
		}
		else
			entries.push_back({ t->getName(), startBuf.str(), endBuf.str() });
	}

	j["entries"] = entries;
	
	// Write to file 
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}

Project JsonFormat::read(const std::string& path) const{
	if(!std::filesystem::exists(path))
		throw std::invalid_argument("File cannot be found \"" + path + "\"\n");
	
    Project project;

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);
  
    std::string nameBuf, lastUpdatedBuf;
    j.at("name").get_to(nameBuf);
	j.at("updated").get_to(lastUpdatedBuf);

    project.setName(nameBuf);
	Timestamp lastUpdated{FileIO::stringToTimepoint(lastUpdatedBuf)};
	//project.setLastUpdated(lastUpdated);                              // Need to correctly set last updated time


    nlohmann::json t;
	j.at("entries").get_to(t);
	
	for (auto& elm : t.items()) {
		nlohmann::json obj = elm.value();
		std::string name, start, end;

		obj.at(0).get_to(name);
		obj.at(1).get_to(start);
		obj.at(2).get_to(end);

		ProjectEntry t{ name };

		t.start (FileIO::stringToTimepoint(start));
		t.end   (FileIO::stringToTimepoint(end));

		project.addEntry(std::make_shared<ProjectEntry>(t));
	}

    return project;
}



FileIOManager::FileIOManager(std::shared_ptr<ProjectFormatter> setProjFormat)
	: projectFormat { setProjFormat }
{

}
FileIOManager::~FileIOManager(){

}
ProjectPtr FileIOManager::readProjectFile(const std::string& path) const{
	if(!std::filesystem::exists(path) || !projectFormat)
		return nullptr;

	try{
		ProjectPtr projBuffer { new Project {projectFormat->read(path)} };
		return projBuffer;
	}
	catch (std::invalid_argument& e){
		std::cerr << e.what();
		return nullptr;
	}

}
std::vector<ProjectPtr> FileIOManager::readDirectory (const std::string& directory) const{
	if(!std::filesystem::exists(directory))
		throw std::invalid_argument("File cannot be found \"" + directory + "\"\n");

	int filesThatCouldNotBeRead{0};
	std::vector<ProjectPtr> projects;
	const std::filesystem::path workingDirectory{directory};
	for(auto const& dir_entry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			projects.push_back(readProjectFile(dir_entry.path().string()));
		}
		catch(std::exception& e){
			++filesThatCouldNotBeRead;
		}
	}

	/* Output number of files that could not be read
	if(filesThatCouldNotBeRead != 0){
		std::cout << "\tThere were " << filesThatCouldNotBeRead << " file(s) that could not be read in \"" << directory << "\"\n";
	}
	*/

	return projects;
}

bool FileIOManager::writeProject(const std::string& directory, const Project& project) const{
	std::string qualifiedDirectory {directory};

	// Ensure backslash
	if(directory.back() != '\\' && directory.back() != '/'){
		qualifiedDirectory += "/";
	}

	if(!std::filesystem::exists(qualifiedDirectory))
			return false;

	std::string fullProjectPath { qualifiedDirectory + std::string{project.getName()} + ".json"};

	if(!projectFormat)
		return false;
	
	return projectFormat->write(fullProjectPath, project);
}
bool FileIOManager::deleteProject(const std::string& path) const {
	return std::filesystem::remove(path);
}


namespace TimeTracker::FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& timeString) {
        std::stringstream ss{ timeString };
        std::tm time{};
		time.tm_isdst = -1;
       	ss >> std::get_time(&time, "%Y-%m-%d%n%H:%M:%S");
	    return std::chrono::system_clock::from_time_t(std::mktime(&time));
    }
}
