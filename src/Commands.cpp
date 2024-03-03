#include "Commands.h"



Command::Command(std::string text) : command{text}
{

}
Command::~Command(){

}
std::string Command::getCommand() const{
    return command;
}
std::string Command::getDescription() const{
    return description;
}



ProjectCommand::ProjectCommand(std::string command, std::weak_ptr<ProjectManager> manager) 
    : Command(command), projectManager {manager}
{

}
ProjectCommand::~ProjectCommand(){

}

SelectProject::SelectProject(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "<name> Selects a project by name.\n";
}
bool SelectProject::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
       if(manager->selectProject(arg))
            return true;
        else{
            std::cout << "\tNo project named \"" << arg << "\" exists.\n";
            return false;
        }
    }
    else
        return false;
}
DeselectProject::DeselectProject(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Deselects the current project.\n";
}
bool DeselectProject::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
       manager->deselectProject();
       return true;
    }
    else
        return false;
}
ListProjects::ListProjects(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "[-p or -e] Lists projects or entries of selected project.\n";
}
bool ListProjects::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){

        // List all project names
        if(arg == "-p"){
            // Ensure map is not empty
            if(manager->getAllProjectNames().empty()){
                std::cout << "\tThere are no tracked projects.\n";
                return true;
            }
            else{
                std::cout << "Current Projects:\n";
                for(std::string& s : manager->getAllProjectNames()){
                    std::cout << '\t' << s << "\n";
                }
                return true;
            }
        }
        else if(arg == "-e"){
            if(manager->getProject() ){
                std::cout << manager->getProject()->printAllEntries().str();
                return true;
            }
            else{
                std::cout << "\tThere is no project selected.\n";
                return false;
            }
        }
        else{
            std::cout << "\tInvalid argument.\n";
            return false;
        }
    }
    else
        return false;
}
CreateProject::CreateProject(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "<name> Creates a new project with the given name and automatically selects it.\n";
}
bool CreateProject::execute(std::string arg){
    if(arg.empty()){
        std::cout << "\tThis command requires one argument\n";
        return false;
    }

    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        ProjectPtr newProject{ new Project (arg)};

        if(manager->addProject(newProject)){
            manager->selectProject(arg);
            return true;
        }
        else{
            std::cout << "\tCould not create project with name \"" << arg << "\". \n\tIt may already exist.\n";
            return false;
        }
    }
    else
        return false;
}
DeleteProject::DeleteProject(std::string command, std::weak_ptr<ProjectManager> manager1, std::weak_ptr<FileIOManager> manager2)
    : ProjectCommand(command, manager1), fileManager {manager2}
{
    this->description = "[name] Deletes the selected project. If no project is selected, then it will delete the project with the given name.\n";
}
bool DeleteProject::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager1 = projectManager.lock()){
         if(std::shared_ptr<FileIOManager> manager2 = fileManager.lock() ){

            std::string projectName;

            // Find proj name from selected project if no arg given
            if(arg.empty()){

                if(manager1->getProject()){
                    projectName = manager1->getProject()->getName(); 
                }
                else{
                     std::cout << "\tThere is no project selected.\n";
                    return false;
                }   
            }
            // Find project name from argument
            else{
               if(manager1->findProject(arg)){
                    projectName = arg;
               }
               else{
                    std::cout << "\tCould not find project \"" << arg << "\"\n";
                    return false;
               }
            }

            // Find fully qualifed path
            std::string fullProjectPath { manager2->getProjectDirectory()};
            fullProjectPath += projectName;
            fullProjectPath += ".json";

            if(std::filesystem::remove(fullProjectPath) && manager1->deleteProject(projectName)){
                std::cout << "\tDeleted project \"" << projectName << "\"\n";
                return true;
            }
            else{
                std::cout << "\tError deleting project \"" << projectName << "\"\n";
                return false;
            }
         }
         else
            return false;
    }
    else
        return false;
}
StartTimer::StartTimer(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "[name] Starts a new timer for the selected project with a given name, if one is provided.\n";
}
bool StartTimer::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject()){
            if(arg.empty())
                if(!manager->getProject()->startTimer()){
                    std::cout << "\tCould not start timer. There may be a timer already running.\n";
                    return false;
                }
                else
                    return true;
            else{
                if(!manager->getProject()->startTimer(arg)){
                     std::cout << "\tCould not start timer. There may be a timer already running.\n";
                    return false;
                }
                else
                    return true;
            }
        }
        else{
            std::cout << "\tThere is no project selected.\n";
            return false;
        }
    }
    else
        return false;
}
EndTimer::EndTimer(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Ends a running timer for the selected project.\n";
}
bool EndTimer::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject()){
            if(!manager->getProject()->endTimer()){
                std::cout << "\tThere is no running timer for the selected project.\n";
                return false;
            }
            else
                return true;
        }
        else{
            std::cout << "\tThere is no project selected.\n";
            return false;
        }
    }
    else
        return false;
}
IsRunning::IsRunning(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Check if a timer is running for either selected project or project with the given name.\n";
}
bool IsRunning::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject()){
            if(manager->getProject()->isTimerRunning()){
                std::cout << "\tThere is a timer running.\n";
                return true;
            }
            else{
                std::cout << "\tNo timer is running for the selected project.\n";
                return false;
            }
        }
        else{
            std::cout << "\tThere is no project selected.\n";
            return false;
        }
    }
    else
        return false;
}

FileIOCommand::FileIOCommand(std::string command, std::weak_ptr<FileIOManager> manager)
    : Command {command}, fileManager{manager}
{

}
FileIOCommand::~FileIOCommand(){

}

Save::Save(std::string command, 
            std::weak_ptr<ProjectManager> setProjManager, 
            std::weak_ptr<FileIOManager> setFileManager, 
            std::weak_ptr<Settings> setSettings)   :   
            Command(command), 
            projectManager{setProjManager}, 
            fileManager{setFileManager}, 
            settings{setSettings}
{
    this->description = "[-p or -s] -p saves all projects, -s saves all settings. No arguments saves everything.\n";
}
bool Save::execute(std::string arg){
    bool saveSettings { false };
    bool saveProjets  { false };


    // If no argument is given, save all
    if(arg.empty()){
        saveSettings = true;
        saveProjets = true;
    }
    else if(arg == "-p")
        saveProjets = true;
    else if(arg == "-s")
        saveSettings = true;
    else{
        std::cout << "\tInvalid argument.\n";
        return false;
    }
    
    // Save all projects
    if(saveProjets){
        int savingErrors { 0 };
        if(std::shared_ptr<FileIOManager> _fileManager = fileManager.lock()){
            if(std::shared_ptr<ProjectManager> _projectManager = projectManager.lock()){
                std::vector<std::shared_ptr<const Project>> projectsBuffer {_projectManager->getAllProjects()};
                for(auto proj : projectsBuffer ){
                    const Project& projectBuffer { *proj.get()};
                    if(!_fileManager->writeProject(projectBuffer))
                        ++savingErrors;
                }

                if(savingErrors != 0){
                    std::cout << "\tThere was an error saving " << savingErrors << " projct(s)\n";
                    return false;
                }
                else{
                    return true;
                }
            }
        }
        else
            return false;
    }
    
    // Save settings
    if(saveSettings){
        if(std::shared_ptr<FileIOManager> _fileManager = fileManager.lock()){
            if(_fileManager->writeSettings(*std::shared_ptr<Settings>{settings.lock()})){
                return true;
            }
            else{
                std::cout << "\tCould not write to settings file \"" << _fileManager->getSettingsPath() << "\"\n";
                return false;
            }
        }
    }
   

    return false;
}


Print::Print(     std::string command, 
            std::weak_ptr<ProjectManager> setProjManager, 
            std::weak_ptr<FileIOManager> setFileManager, 
            std::shared_ptr<Settings> setSettings
        ) :
            Command(command), 
            projectManager{setProjManager}, 
            fileManager{setFileManager}, 
            settings{setSettings}
{
    this->description = "<arg> -t print total time of selected project, -d print project read/write directory, -s print settings\n";
}
bool Print::execute(std::string arg){
    if(arg.empty()){
        std::cout << "\tThis command requires one argument.\n";
        return false;
    }
    else if(arg == "-t"){
        if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
            if(manager->getProject()){
                std::cout << "\tTotal time for this project is: " << manager->getProject()->printTotalTime() <<"  [HH:MM:SS]\n";
                return true;
            }
            else{
                std::cout << "\tThere is no project selected.\n";
                return false;
            }   
        }
        else
            return false;
    }
    else if(arg == "-d"){
        if(std::shared_ptr<FileIOManager> manager = fileManager.lock()){
            std::cout << "\tProject directory is at \"" << manager->getProjectDirectory() << "\"\n"; 
        }
        else
            return false;
        
        return false;
    }
    else if(arg == "-s"){
        if(std::shared_ptr<Settings> set = settings.lock()){
            std::cout << "\tProject Directory: \"" << set->getProjectDirectory() << "\"\n";
            std::cout << "\tHour Offset: " << set->getHourOffset() << "\n";
            std::cout << "\tVerbose Mode: " << std::boolalpha << set->getVerbose() << "\n";
            return true;
        }
        else
            return false;
    }
    else{
        std::cout << "\tInvalid argument.\n";
        return false;
    }
}

RefreshSettings::RefreshSettings(std::string command,  std::weak_ptr<Settings> setSettings,  std::weak_ptr<FileIOManager> manager)
:   Command{command}, settings{setSettings}, fileManager{manager}
{
    this->description = "Applies current settings.\n";

}
bool RefreshSettings::execute(std::string arg){
    if(std::shared_ptr<Settings> set = settings.lock()){
        Timestamp::setHourOffset(set->getHourOffset());

        if(std::shared_ptr<FileIOManager> manager = fileManager.lock()){
            manager->setProjectDirectory(set->getProjectDirectory());

            return true;
        }
        else
            return false;
        
    }
    else
        return false;
}
SetVerbose::SetVerbose(std::string command,  std::weak_ptr<Settings> setSettings)
:   Command{command}, settings{setSettings}
{
    this->description = "<value> Set verbose mode on/off. \"true\" or \"false\"\n";

}
bool SetVerbose::execute(std::string arg){
    if(std::shared_ptr<Settings> set = settings.lock()){
        if(arg.empty()){
            std::cout << "\tThis command requires one argument.\n";
            return false;
        }
        else if (arg == "true" || arg == "TRUE"){
            set->setVerbose(true);
            return true;
        }
        else if (arg == "false" || arg == "FALSE"){
            set->setVerbose(false);
            return true;
        }
        else{
            std::cout << "\tInvalid argument.\n";
            return false;
        }
    }
    else
        return false;
}
SetHourOffset::SetHourOffset(std::string command,  std::weak_ptr<Settings> setSettings)
:   Command{command}, settings{setSettings}
{
    this->description = "<value> Set hour offset for timers.\n";

}
bool SetHourOffset::execute(std::string arg){
    if(std::shared_ptr<Settings> set = settings.lock()){
        try{
            int offset {std::stoi(arg)};
            set->setHourOffset(offset);
            return true;
        }
        catch(std::exception& e){
            std::cout << "\tInvalid argument.\n";
            return false;
        }
    }
    else
        return false;
}
SetProjectDirectory::SetProjectDirectory(std::string command,  std::weak_ptr<Settings> setSettings)
:   Command{command}, settings{setSettings}
{
    this->description = "<directory> Set the read/write directory for projects.\n";

}
bool SetProjectDirectory::execute(std::string arg){
    if(std::shared_ptr<Settings> set = settings.lock()){
        if(set->setProjectDirectory(arg)){
            return true;
        }
        else{
            std::cout << "\tCould not set project save directory to \"" << arg << "\"\n";
            return false;
        }
    }
    else
        return false;
}
ClearScreen::ClearScreen(std::string command)
:   Command{command}
{
    this->description = "Clears the screen.\n";
}
bool ClearScreen::execute(std::string arg){
    #ifdef __linux__
    system("clear -x");
    #endif

    #ifdef _Win32
    system("cls");
    #endif

    return true;
}