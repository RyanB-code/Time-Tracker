#pragma once

#include <memory>

#include "Project.h"
#include "FileIO.h"
#include "Commands.h"

#include <json.h>

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
        std::unordered_map<std::string, std::unique_ptr<Command>> commands;
        std::shared_ptr<ProjectManager>     projectManager;
        std::shared_ptr<FileIOManager>      fileManager;
        std::shared_ptr<Settings>           settings;

        std::string settingsPath;

        void handleArguments    (std::vector<std::string>& args);
        void handleSettingsFile (const std::string& path);
    };
    
    std::string determineSaveDirectory();
    std::string determineRCPath();
}
