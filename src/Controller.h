#include "Project.h"
#include "FileIO.h"
#include <sstream>
#include <format>

class Command{
public:
    Command(std::string text);
    ~Command();

    virtual bool execute(std::string arg="") = 0;
    std::string getCommand() const;
    std::string getDescription() const;
private:
    std::string command { };
protected:
    std::string description { };
};


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
class CreateProject : public ProjectCommand{
public:
    CreateProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};
class DeleteProject : public ProjectCommand{
public:
    std::weak_ptr<FileIOManager> fileManager;

    DeleteProject(std::string command, std::weak_ptr<ProjectManager> manager1, std::weak_ptr<FileIOManager> manager2);

    bool execute(std::string arg="") override; 
};
class StartTimer : public ProjectCommand{
public:
    StartTimer(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};
class EndTimer : public ProjectCommand{
public:
    EndTimer(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};
class IsRunning : public ProjectCommand{
public:
    IsRunning(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};
class TotalTime : public ProjectCommand{
public:
    TotalTime(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::string arg="") override; 
};


class FileIOCommand : public Command {
public:
    std::weak_ptr<FileIOManager> fileManager;

    FileIOCommand(std::string command, std::weak_ptr<FileIOManager> manager);
    ~FileIOCommand();
};

class SaveAllProjects : public FileIOCommand {
public:
    std::weak_ptr<ProjectManager> projectManager;

    SaveAllProjects(std::string command, std::weak_ptr<FileIOManager> fileManager, std::weak_ptr<ProjectManager> projectManager);
    
    bool execute(std::string arg="") override;
};

class PrintFileIODirectory : public FileIOCommand {
public:
    PrintFileIODirectory(std::string command, std::weak_ptr<FileIOManager> fileManager);
    
    bool execute(std::string arg="") override;
};
