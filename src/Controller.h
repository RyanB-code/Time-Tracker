#include "Project.h"
#include "FileIO.h"
#include <sstream>

class Command;

class EventHandler{
public:
    EventHandler(std::shared_ptr<ProjectManager> manager, std::shared_ptr<ProjectFormatter> format);
    ~EventHandler();

    bool run();
    bool addCommand(std::unique_ptr<Command> command);

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    std::shared_ptr<ProjectManager>     projectManager;
    std::shared_ptr<ProjectFormatter>   saveFormat;

    bool setup();
    void handleArguments(std::vector<std::string>& args);
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
    std::weak_ptr<ProjectManager> projectManager;

    ProjectCommand(std::string command, std::weak_ptr<ProjectManager> manager);
    ~ProjectCommand();
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
class ListProjects : public ProjectCommand{
public:
    ListProjects(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override;
};
class ListEntries : public ProjectCommand{
public:
    ListEntries(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};



class FileIOCommand : public Command {
public:
    std::weak_ptr<ProjectFormatter> saveFormat;

    FileIOCommand(std::string command, std::weak_ptr<ProjectFormatter> format);
    ~FileIOCommand();
};

class ReadFile : public FileIOCommand {
public:
    std::weak_ptr<ProjectManager> projectManager;

    ReadFile(std::string command, std::weak_ptr<ProjectFormatter> format, std::weak_ptr<ProjectManager> manager);
    
    bool execute(std::string arg="") override;
};
