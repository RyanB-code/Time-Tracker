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
bool JsonFormat::write(std::string path, const Project& project){
    json j{
		{"name", project.getName()},
		{"updated", project.getLastUpdated().printRaw()}
	};

	int i{ 0 };
	nlohmann::json entries = nlohmann::json::array();

	for (const auto& t : project.getEntries()) {
		std::ostringstream s1; s1 << t->getRawStartTime();
		std::ostringstream s2; s2 << t->getRawEndTime();

		if (t->getName() == "") {
			std::ostringstream name;
			name << "Entry " << i;

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

Project JsonFormat::read(std::string path){
    Project project;

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);
  
    std::cout << "JSON HERE\n\n" << std::setw(1) << std::setfill('\t') << j;

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

namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time) {
        std::stringstream ss{ time };
        std::tm _tm{};
        ss >> std::get_time(&_tm, "%Y-%m-%d %H:%M:%S");
	    return std::chrono::system_clock::from_time_t(std::mktime(&_tm));
    }
}
