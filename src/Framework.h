#pragma once

#include <memory>

#include "Project.h"
#include "FileIO.h"
#include "Commands.h"

#include <json.h>
#include <queue>

namespace TimeTracker{
    class Framework{
    public:
        Framework(  std::shared_ptr<ProjectManager> setProjectManager, 
                    std::shared_ptr<FileIOManager>  setFileManager,
                    std::string                     setSettingsPath,
                    std::string                     setDefaultProjectDirectory
                );
        ~Framework();

        bool setup();
        bool run();

        bool addCommand(std::unique_ptr<Command> command);

    private:
        std::shared_ptr<ProjectManager>                             projectManager;
        std::shared_ptr<FileIOManager>                              fileManager;
        std::shared_ptr<Settings>                                   settings;

        std::queue<std::string>                                     commandQueue;
        std::unordered_map<std::string, std::unique_ptr<Command>>   commands;

        std::string settingsPath;

        void    getInput            ();             // Displays prompt and waits for user input
        int     handleCommandQueue  ();             // Returns: 0 - normal, 1- exit,
        void    readRCFile          (const std::string& path);

        void    setupCommands       ();
        bool    confirmExit         (); // Checks if there is a running timer before exit, returns true if should exit
    };
    
    std::string determineSaveDirectory();
    std::string determineRCPath();
}
