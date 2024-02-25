#include <memory>

#include "Project.h"
#include "FileIO.h"
#include "Commands.h"

#include <json.h>

class Framework{
public:
    Framework(std::shared_ptr<ProjectManager> manager1, std::shared_ptr<FileIOManager> manager2, int hourOffset=0);
    ~Framework();

    bool run();
    bool addCommand(std::unique_ptr<Command> command);

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    std::shared_ptr<ProjectManager>     projectManager;
    std::shared_ptr<FileIOManager>      fileManager;

    bool setup();
    void handleArguments(std::vector<std::string>& args);
};

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