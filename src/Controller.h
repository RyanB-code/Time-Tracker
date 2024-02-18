#include "Project.h"
#include "FileIO.h"

class Command;

class EventHandler{
public:
    EventHandler(std::shared_ptr<ProjectManager> manager, std::shared_ptr<ProjectFormatter> format);
    ~EventHandler();

    bool run();
    bool addCommand(std::unique_ptr<Command> command);

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    std::shared_ptr<ProjectManager> projectManager;
    std::shared_ptr<ProjectFormatter> saveFormat;


    bool setup();
};


class Command{
public:
    Command(std::string text);
    ~Command();

    virtual bool execute(std::string arg="") = 0;
    std::string getCommand();
private:
    std::string command { };
};




class ProjectCommand : public Command{
public:
    ProjectCommand(std::string command, std::weak_ptr<ProjectManager> manager);
    ~ProjectCommand();

    std::weak_ptr<ProjectManager> projectManager;
};

class SelectProject : public ProjectCommand{
public:
    SelectProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override;
};
class DeselectProject : public ProjectCommand{
public:
    DeselectProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override;
};




class FileIOCommand : public Command {
public:
    FileIOCommand(std::string command, std::unique_ptr<ProjectFormatter> format);
    ~FileIOCommand();

    std::unique_ptr<ProjectFormatter> saveFormat;
};

