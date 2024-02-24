#include <memory>

#include "Project.h"
#include "FileIO.h"
#include "Commands.h"

class Command;

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