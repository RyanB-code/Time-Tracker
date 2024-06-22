#include "Settings.h"

using json = nlohmann::json;

Settings::Settings(std::string setProject, bool setVerbose, int setHourOffset, uint8_t setEntryNameWidth)
    :   verbose { setVerbose },
        hourOffset { setHourOffset },
        entryNameWidth {setEntryNameWidth}

{
    if(!setProjectDirectory(setProject)){
        std::invalid_argument e{"Project directory could not be found/created."};
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
void Settings::setEntryNameWidth (uint8_t set){
    entryNameWidth = set;
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
uint8_t    Settings::getEntryNameWidth() const{
    return entryNameWidth;
}


