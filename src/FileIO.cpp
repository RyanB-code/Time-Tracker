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

    return false;
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



FileIOManager::FileIOManager(std::shared_ptr<ProjectFormatter> format)
	: saveFormat { format }
{
}
FileIOManager::~FileIOManager(){

}
ProjectPtr FileIOManager::readFile(std::string path) const{
	if(!std::filesystem::exists(path))
		return nullptr;

	if(saveFormat){ 
		ProjectPtr projBuffer { new Project {saveFormat->read(path)}};
		return projBuffer;
    }
    else
        return nullptr;

    return nullptr;
}
std::vector<ProjectPtr> FileIOManager::readDirectory (std::string directory) const{
	if(directory.empty()){
		directory = saveDirectory;
	}
	else{
		if(!std::filesystem::exists(directory))
			return std::vector<ProjectPtr>{};		// Should find better way to signal the directory is bad 
	}

	std::vector<ProjectPtr> projects;
	const std::filesystem::path workingDirectory{directory};
	for(auto const& dir_entry : std::filesystem::directory_iterator(workingDirectory)){
		projects.push_back(readFile(dir_entry.path().string()));
	}

	return projects;
}

bool FileIOManager::writeProject(const Project& project, std::string path) const{
	if(path.empty()){
		path = saveDirectory;
		path += project.getName();
		path += ".json";
	}
	else{
		if(!std::filesystem::exists(path))
			return false;
	}

	if(saveFormat){
		return saveFormat->write(path, project);
	}
	else
		return false;
}

std::string_view FileIOManager::getDirectory() const {
	return saveDirectory;
}

bool FileIOManager::setDirectory(std::string path){
	// Ensure backslash
	if(path.back() != '\\' && path.back() != '/'){
		path += "/";
	}

	if(std::filesystem::exists(path)){
		saveDirectory = path;
		return true;
	}
	// Create directory if it does not exist
	else{
		if(std::filesystem::create_directory(path)){
			saveDirectory = path;
			return true;
		}
		else
			return false;
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
