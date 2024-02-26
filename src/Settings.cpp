#include "Settings.h"

using json = nlohmann::json;

Settings::Settings(std::string setProject, std::string setSettings, bool setVerbose, int setHourOffset)
    :  verbose { setVerbose },
        hourOffset { setHourOffset }
{
    if(!setProjectDirectory(setProject) || !setSettingsPath(setSettings)){
        std::invalid_argument e{"Settings file path or desired project directory could not be found/created."};
        throw e;
    }
}
Settings::~Settings(){

}
void Settings::setVerbose(bool set){
    verbose = set;
}
void Settings::setHourOffset(int set){
    hourOffset = set;
}
bool Settings::setProjectDirectory(std::string set){

    // Ensure backslash
	if(set.back() != '\\' && set.back() != '/'){
		set += "/";
	}

    // If it exists, set member variable
    if(std::filesystem::exists(set)){
        projectDirectory = set;
        return true;
    }
    else{

        // Create directory if it does not exist
        try{
			if(std::filesystem::create_directory(set)){
				projectDirectory = set;
				return true;
			}
			else
				return false;
		}
		catch(std::filesystem::filesystem_error& e){
			return false;
		}

        return false;
    }
}
bool Settings::setSettingsPath(std::string set){

    // If file exists, set member variable
    if(std::filesystem::exists(set)){
        settingsPath = set;
        return true;
    }
    else{
        std::ofstream settingsFile {set, std::ios::trunc};       // Create file

        // Check if it exists again
        if(std::filesystem::exists(set)){
            settingsPath = set;

            // Once created, write to it
            if(writeSettingsFile()){
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    return false;
}
bool Settings::getVerbose() const{
    return verbose;
}
int Settings::getHourOffset() const{
    return hourOffset;
}
std::string Settings::getProjectDirectory() const{
    return projectDirectory;
}
std::string Settings::getSettingsPath() const{
    return settingsPath;
}
bool Settings::readSettingsFile(){
    std::ifstream inputFile{ settingsPath, std::ios::in };
    json j = json::parse(inputFile);

    bool verboseBuffer;
    int hourOffsetBuffer;
    std::string projectDirectoryBuffer, settingsPathBuffer;

    j.at("verbose").get_to(verboseBuffer);
	j.at("hour-offset").get_to(hourOffsetBuffer);
    j.at("project-directory").get_to(projectDirectoryBuffer);
    j.at("settings-path").get_to(settingsPathBuffer);

    setVerbose(verboseBuffer);
    setHourOffset(hourOffsetBuffer);
    
    if(!setProjectDirectory(projectDirectoryBuffer) || !setSettingsPath(settingsPathBuffer)){
        return false;
    }
    else
        return true;
    
    return false;
}
bool Settings::writeSettingsFile() const{
    json j{ };
    j["verbose"] = verbose;
    j["hour-offset"] = hourOffset;
    j["project-directory"] = projectDirectory;
    j["settings-path"] = settingsPath;

    // Write to file here
    std::ofstream file{settingsPath};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
