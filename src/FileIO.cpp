#include "FileIO.h"

using json = nlohmann::json;

ProjectFormatter::ProjectFormatter(){

}
ProjectFormatter::~ProjectFormatter(){

}


JsonFormat::JsonFormat(){

}
JsonFormat::~JsonFormat(){

}
bool JsonFormat::write(std::string path, const Project& project) const{
    json j{
		{"name", project.getName()},
		{"updated", project.getLastUpdated().printRaw()}
	};

	int i{ 0 };
	nlohmann::json entries = nlohmann::json::array();
	for (const auto& t : project.getEntries()) {
		std::ostringstream s1; s1 << t->getRawStartTime();
		std::ostringstream s2; s2 << t->getRawEndTime();

		if (t->getName() == "Unnamed Entry") {
			std::ostringstream name;
			name << "Entry " << t->getID();

			entries.push_back({ name.str(), s1.str(), s2.str() } );
		}
		else
			entries.push_back({ t->getName(), s1.str(), s2.str()});

		++i;
	}

	j["entries"] = entries;

    // Write to file here
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}

Project JsonFormat::read(std::string path) const{
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
ProjectPtr FileIOManager::readProjectFile(std::string path) const{
	if(!std::filesystem::exists(path))
		return nullptr;

	if(projectFormat){ 
		ProjectPtr projBuffer { new Project {projectFormat->read(path)}};
		return projBuffer;
    }
    else
        return nullptr;

    return nullptr;
}
std::vector<ProjectPtr> FileIOManager::readDirectory (std::string directory) const{
	if(!std::filesystem::exists(directory))
		return std::vector<ProjectPtr>{};		// Should find better way to signal the directory is bad 

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

bool FileIOManager::writeProject(const Project& project, std::string directory) const{

	// Ensure backslash
	if(directory.back() != '\\' && directory.back() != '/'){
		directory += "/";
	}

	if(!std::filesystem::exists(directory))
			return false;

	std::string fullProjectPath { directory + std::string{project.getName()} + ".json"};

	if(projectFormat){
		return projectFormat->write(fullProjectPath, project);
	}
	else
		return false;
}
bool FileIOManager::deleteProject(std::string path) const {
	return std::filesystem::remove(path);
}


namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time) {
        std::stringstream ss{ time };
        std::tm _tm{};
		_tm.tm_isdst = -1;
       	ss >> std::get_time(&_tm, "%Y-%m-%d%n%H:%M:%S");
	    return std::chrono::system_clock::from_time_t(std::mktime(&_tm));
    }
}
