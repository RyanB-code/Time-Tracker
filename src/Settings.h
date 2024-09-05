#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#ifdef RELEASE
#include <Config.h>
#endif

#include "json.h"

namespace TimeTracker {
    class Settings{
    public:
        Settings(   std::string setProjectDirectory, 
                    bool setVerbose=false, 
                    uint8_t setEntryNameWidth=20,
                    uint8_t setEntriesPerPage=5
                );
        ~Settings();

        void setVerbose             (bool set);
        bool setProjectDirectory    (std::string set);
        void setEntryNameWidth      (uint8_t set);
        void setEntriesPerPage      (uint8_t set);

        bool        getVerbose          ()   const;
        std::string getProjectDirectory ()   const;
        uint8_t     getEntryNameWidth   ()   const;
        uint8_t     getEntriesPerPage   ()   const;

    private:
        bool        verbose             { false };
        std::string projectDirectory    { };        // Where projects will be read/written to
        uint8_t     entryNameWidth      { };        // Width of text field for entry names
        uint8_t     entriesPerPage      { };        // Entries shown per page
    };

    std::string getVersion();           // Returns version in format 1.0.1
    std::string getAllProjectInfo();    // Returns copyright information, author (me) and version information
}