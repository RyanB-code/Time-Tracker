#pragma once

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
                int setHourOffset=0,
                uint8_t setEntryNameWidth=20
            );
    ~Settings();

    void setVerbose             (bool set);
    void setHourOffset          (int set);
    bool setProjectDirectory    (std::string set);
    void setEntryNameWidth      (uint8_t set);

    bool        getVerbose()            const;
    int         getHourOffset()         const;
    std::string getProjectDirectory()   const;
    uint8_t     getEntryNameWidth()     const;

private:
    bool verbose { false };
    int hourOffset { 0 };
    std::string projectDirectory { };   // Where projects will be read/written to
    uint8_t entryNameWidth { };
};