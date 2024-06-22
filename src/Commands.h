#include "Project.h"
#include "FileIO.h"
#include "Settings.h"

#include <Config.h>

#include <sstream>
#include <format>

class Command{
public:
    Command(std::string text);
    virtual ~Command();

    virtual bool execute(std::vector<std::string> args) = 0;
    std::string getCommand() const;
    std::string getDescription() const;
private:
    std::string command { };
protected:
    std::string description { };
};

class ProjectCommand : public Command{
public:
    std::weak_ptr<ProjectManager>   projectManager;

    ProjectCommand(std::string command, std::weak_ptr<ProjectManager> manager);
    virtual ~ProjectCommand();
};


// MARK: Project Command
class SelectProject : public ProjectCommand{
public:
    SelectProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::vector<std::string> args) override;
};
class DeselectProject : public ProjectCommand{
public:
    DeselectProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::vector<std::string> args) override;
};
class List : public ProjectCommand{
public:
    std::weak_ptr<Settings> settings;

    List(std::string command, std::weak_ptr<ProjectManager> manager, std::weak_ptr<Settings> setSettings);

    bool execute(std::vector<std::string> args) override;
};
class CreateProject : public ProjectCommand{
public:
    CreateProject(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::vector<std::string> args) override;
};
class DeleteProject : public ProjectCommand{
public:
    std::weak_ptr<FileIOManager>    fileManager;
    std::weak_ptr<Settings>         settings;

    DeleteProject(std::string command, std::weak_ptr<ProjectManager> manager1, std::weak_ptr<FileIOManager> manager2, std::weak_ptr<Settings> set);

    bool execute(std::vector<std::string> args) override;
};
class StartTimer : public ProjectCommand{
public:
    StartTimer(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::vector<std::string> args) override;
};
class EndTimer : public ProjectCommand{
public:
    EndTimer(std::string command, std::weak_ptr<ProjectManager> manager);

    bool execute(std::vector<std::string> args) override;
};


// MARK: Regular Commands
class FileIOCommand : public Command {
public:
    std::weak_ptr<FileIOManager> fileManager;

    FileIOCommand(std::string command, std::weak_ptr<FileIOManager> manager);
    virtual ~FileIOCommand();
};
class Save : public Command {
public:
    std::weak_ptr<ProjectManager> projectManager;
    std::weak_ptr<FileIOManager> fileManager;
    std::weak_ptr<Settings> settings;

    Save(   std::string command, 
            std::weak_ptr<ProjectManager> setProjManager, 
            std::weak_ptr<FileIOManager> setFileManager, 
            std::weak_ptr<Settings> setSettings
        );
    
    bool execute(std::vector<std::string> args) override;
};

class Print : public Command {
public:
    std::weak_ptr<ProjectManager> projectManager;
    std::weak_ptr<FileIOManager> fileManager;
    std::weak_ptr<Settings> settings;

    Print(  std::string command, 
            std::weak_ptr<ProjectManager> setProjManager, 
            std::weak_ptr<FileIOManager> setFileManager, 
            std::shared_ptr<Settings> setSettings
        );
    
    bool execute(std::vector<std::string> args) override;
};
class Set : public Command {
public:
    std::weak_ptr<Settings> settings;

    Set(std::string command, std::weak_ptr<Settings> setSettings);

    bool execute(std::vector<std::string> args);
};

class ClearScreen : public Command{
public:
    ClearScreen(std::string command);

    bool execute(std::vector<std::string> args) override;
};
