#include "Settings.h"

using json = nlohmann::json;
using namespace TimeTracker;

Settings::Settings( std::string setProject, 
                    bool setVerbose,
                    bool setAutoSave, 
                    uint8_t setEntryNameWidth,
                    uint8_t setEntriesPerPage
                )
                :   verbose         { setVerbose },
                    autoSave        { setAutoSave },
                    entryNameWidth  { setEntryNameWidth },
                    entriesPerPage  { setEntriesPerPage }

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
void Settings::setAutoSave(bool set){
    autoSave = set;
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
void Settings::setEntriesPerPage (uint8_t set){
    entriesPerPage = set;
}


bool Settings::getVerbose() const{
    return verbose;
}
bool Settings::getIsAutoSaveOn() const {
    return autoSave;
}
std::string Settings::getProjectDirectory() const{
    return projectDirectory;
}
uint8_t    Settings::getEntryNameWidth() const{
    return entryNameWidth;
}
uint8_t Settings::getEntriesPerPage() const{
    return entriesPerPage;
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
    
    os << "Time Tracker by Bradley Ryan\nVersion " << getVersion() << " - ";

    #ifdef DEBUG
        os << "Debug";
    #endif

    #ifdef RELEASE
        os << "Release";
    #endif

    return os.str();
}

