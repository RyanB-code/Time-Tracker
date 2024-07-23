#include "Settings.h"

using json = nlohmann::json;
using namespace TimeTracker;

Settings::Settings( std::string setProject, 
                    bool setVerbose, 
                    int setHourOffset, 
                    uint8_t setEntryNameWidth
                )
                :   verbose         { setVerbose },
                    hourOffset      { setHourOffset },
                    entryNameWidth  {setEntryNameWidth}

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

    // Create directory if it does not exist
    try{
        if(!std::filesystem::create_directory(set))
            return false;
        
        projectDirectory = set;
        return true;
    }
    catch(std::filesystem::filesystem_error& e){
        return false;
    }

    return false;
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


std::string TimeTracker::getVersion(){
    std::ostringstream os;

    #if defined RELEASE
    os <<   TimeTracker_VERSION_MAJOR << "." <<
            TimeTracker_VERSION_MINOR << "." <<
            TimeTracker_VERSION_PATCH;
    #elif defined DEBUG
        os << "DEBUG";
    #else
        os << "NULL";
    #endif

    return os.str();
}
std::string TimeTracker::getAllProjectInfo(){
    std::ostringstream os;
    
    os << "\tTime Tracker by Bradley Ryan\n\tVersion " << getVersion() << " - ";

    #ifdef DEBUG
        os << "Debug";
    #endif

    #ifdef RELEASE
        os << "Release";
    #endif

    os << "\n\tBuilt on " << __TIMESTAMP__ << "\n"; 

    return os.str();
}

