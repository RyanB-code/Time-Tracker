#include <memory>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "json.h"

class Settings{
public:
    Settings(std::string setProjectDirectory, std::string setSettingsPath, bool setVerbose=false, int setHourOffset=0);
    ~Settings();

    void setVerbose             (bool set);
    void setHourOffset          (int set);
    bool setProjectDirectory    (std::string set);
    bool setSettingsPath        (std::string set);

    bool        getVerbose()            const;
    int         getHourOffset()         const;
    std::string getProjectDirectory()   const;
    std::string getSettingsPath()       const;

    bool readSettingsFile();
    bool writeSettingsFile()    const;

private:
    bool verbose { false };
    int hourOffset { 0 };
    std::string projectDirectory { };   // Where projects will be read/written to
    std::string settingsPath { };       // Where to read the settings file
};