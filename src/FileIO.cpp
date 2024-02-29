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



SettingsFormatter::SettingsFormatter(){

}
SettingsFormatter::~SettingsFormatter(){

}
SettingsJsonFormat::SettingsJsonFormat(){

}
SettingsJsonFormat::~SettingsJsonFormat(){

}
bool SettingsJsonFormat::write (std::string path, const Settings& settings) const{
 	json j{ };
    j["verbose"] = settings.getVerbose();
    j["hour-offset"] = settings.getHourOffset();
    j["project-directory"] = settings.getProjectDirectory();

    // Write to file here
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
Settings SettingsJsonFormat::read(std::string path) const{

	std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    bool verboseBuffer;
    int hourOffsetBuffer;
    std::string projectDirectoryBuffer;

    j.at("verbose").get_to(verboseBuffer);
	j.at("hour-offset").get_to(hourOffsetBuffer);
    j.at("project-directory").get_to(projectDirectoryBuffer);

	return Settings {projectDirectoryBuffer, verboseBuffer, hourOffsetBuffer};
}










FileIOManager::FileIOManager(std::shared_ptr<ProjectFormatter> setProjFormat, std::shared_ptr<SettingsFormatter> setSettingsFormat)
	: projectFormat { setProjFormat }, settingsFormat{ setSettingsFormat }
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
std::shared_ptr<Settings> FileIOManager::readSettingsFile(std::string path) const{

	if(path.empty()){
		path = settingsPath;
	}

	if(!std::filesystem::exists(path))
		return nullptr;

	if(settingsFormat){ 
		try{
			Settings settingsBuffer {settingsFormat->read(path)};
			return std::make_shared<Settings>(settingsBuffer);
		}
		catch(std::exception& e){
			return nullptr;
		}
    }
    else
        return nullptr;

    return nullptr;
}

std::vector<ProjectPtr> FileIOManager::readDirectory (std::string directory) const{
	if(directory.empty()){
		directory = projectDirectory;
	}
	else{
		if(!std::filesystem::exists(directory))
			return std::vector<ProjectPtr>{};		// Should find better way to signal the directory is bad 
	}

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

bool FileIOManager::writeProject(const Project& project, std::string path) const{
	if(path.empty()){
		path = projectDirectory;
		path += project.getName();
		path += ".json";
	}
	else{
		if(!std::filesystem::exists(path))
			return false;
	}

	if(projectFormat){
		return projectFormat->write(path, project);
	}
	else
		return false;
}
bool FileIOManager::writeSettings(const Settings& settings, std::string path) const{
	if(path.empty()){
		path = settingsPath;
	}
	else{
		if(!std::filesystem::exists(path))
			return false;
	}

	if(settingsFormat){
		return settingsFormat->write(path, settings);
	}
	else
		return false;
}

std::string FileIOManager::getHomeDirectory() const {
	return homeDirectory;
}
std::string FileIOManager::getProjectDirectory() const {
	return projectDirectory;
}
std::string FileIOManager::getSettingsPath() const {
	return settingsPath;
}
bool FileIOManager::setHomeDirectory(std::string path){

	bool homeDirectoryExists{false};

	// Ensure backslash
	if(path.back() != '\\' && path.back() != '/'){
		path += "/";
	}

	if(std::filesystem::exists(path)){
		homeDirectory = path;
		homeDirectoryExists = true;
	}
	// Create directory if it does not exist
	else{
		try{
			if(std::filesystem::create_directory(path)){
				homeDirectory = path;
				homeDirectoryExists = true;
			}
			else
				return false;
		}
		catch(std::filesystem::filesystem_error& e){
			return false;
		}
	}

	if(homeDirectoryExists){
		settingsPath = homeDirectory + "Settings.json";
		return true;
	}
	else
		return false;
}
bool FileIOManager::setProjectDirectory(std::string path){
	// Ensure backslash
	if(path.back() != '\\' && path.back() != '/'){
		path += "/";
	}

	if(std::filesystem::exists(path)){
		projectDirectory = path;
		return true;
	}
	// Create directory if it does not exist
	else{
		try{
			if(std::filesystem::create_directory(path)){
				projectDirectory = path;
				return true;
			}
			else
				return false;
		}
		catch(std::filesystem::filesystem_error& e){
			return false;
		}
	}
}




namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time) {
        std::stringstream ss{ time };
        std::tm _tm{};
		_tm.tm_isdst = 0;
       	ss >> std::get_time(&_tm, "%Y-%m-%d%n%H:%M:%S");
	    return std::chrono::system_clock::from_time_t(std::mktime(&_tm));
    }
}
