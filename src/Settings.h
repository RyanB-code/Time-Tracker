#include <memory>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "json.h"

class Settings{
public:
    Settings(   std::string setProjectDirectory, 
                bool setVerbose=false, 
                int setHourOffset=0
            );
    ~Settings();

    void setVerbose             (bool set);
    void setHourOffset          (int set);
    bool setProjectDirectory    (std::string set);

    bool        getVerbose()            const;
    int         getHourOffset()         const;
    std::string getProjectDirectory()   const;

private:
    bool verbose { false };
    int hourOffset { 0 };
    std::string projectDirectory { };   // Where projects will be read/written to
};