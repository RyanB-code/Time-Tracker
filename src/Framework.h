#pragma once

#include <memory>

#include "Project.h"
#include "FileIO.h"
#include "Commands.h"

#include <json.h>
#include <queue>
#include <thread>

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

        void    getInput            (std::stop_source savingThread);    // Requests stop after input is accepted
        void    getInput            ();                                 // Does not request stop after input is accepted

        int     handleCommandQueue  ();             // Returns: 0 - normal, 1- exit,
        void    readRCFile          (const std::string& path);

        void    setupCommands       ();
        bool    confirmExit         (); // Checks if there is a running timer before exit, returns true if should exit

    };
    
    std::string determineSaveDirectory();
    std::string determineRCPath();

    void saveEveryMinute (  std::stop_token stopSaving,  
                            std::shared_ptr<ProjectManager> projectManager, 
                            std::shared_ptr<FileIOManager>  fileManager,
                            std::shared_ptr<Settings>       settings
                        );
}
