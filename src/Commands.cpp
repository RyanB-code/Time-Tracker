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
    this->description = "<string> Selects a project by name.\n";
}
bool SelectProject::execute(std::vector<std::string> args){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
       if(manager->selectProject(args[0]))
            return true;
        else{
            std::cout << "\tNo project named \"" << args[0] << "\" exists.\n";
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
bool DeselectProject::execute(std::vector<std::string> args){
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
    this->description = "<arg>\n-p Lists projects\n-e Lists entries of selected project.\n";
}
bool ListProjects::execute(std::vector<std::string> args){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){

        // List all project names
        if(args[0] == "-p"){
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
        else if(args[0] == "-e"){
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
    this->description = "<string> Creates a new project with the given name and automatically selects it.\n";
}
bool CreateProject::execute(std::vector<std::string> args){
    if(args.empty()){
        std::cout << "\tThis command requires one argument\n";
        return false;
    }

    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        ProjectPtr newProject{ new Project (args[0])};

        if(manager->addProject(newProject)){
            manager->selectProject(args[0]);
            return true;
        }
        else{
            std::cout << "\tCould not create project with name \"" << args[0] << "\". \n\tIt may already exist.\n";
            return false;
        }
    }
    else
        return false;
}
DeleteProject::DeleteProject(std::string command, std::weak_ptr<ProjectManager> manager1, std::weak_ptr<FileIOManager> manager2, std::weak_ptr<Settings> set)
    : ProjectCommand(command, manager1), fileManager {manager2}, settings { set }
{
    this->description = "[name] Deletes the selected project. If no project is selected, then it will delete the project with the given name.\n";
}
bool DeleteProject::execute(std::vector<std::string> args){
    if(std::shared_ptr<ProjectManager> manager1 = projectManager.lock()){
         if(std::shared_ptr<FileIOManager> manager2 = fileManager.lock() ){
            if(std::shared_ptr<Settings> set = settings.lock()){

                std::string projectName;

                // Find proj name from selected project if no arg given
                if(args.empty()){

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
                    std::string find {args[0]};
                    if(manager1->findProject(find)){
                            projectName = find;
                    }
                    else{
                            std::cout << "\tCould not find project \"" << find << "\"\n";
                            return false;
                    }
                }

                // Find fully qualifed path
                std::string fullProjectPath { set->getProjectDirectory()};
                fullProjectPath += projectName;
                fullProjectPath += ".json";

                if(manager2->deleteProject(fullProjectPath) && manager1->deleteProject(projectName)){
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
    else
        return false;
}
StartTimer::StartTimer(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "[string] Starts a new timer for the selected project with a given name, if one is provided.\n";
}
bool StartTimer::execute(std::vector<std::string> args){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject()){
            if(args.empty())
                if(!manager->getProject()->startTimer()){
                    std::cout << "\tCould not start timer. There may be a timer already running.\n";
                    return false;
                }
                else
                    return true;
            else{
                if(!manager->getProject()->startTimer(args[0])){
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
bool EndTimer::execute(std::vector<std::string> args){
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
    this->description = "Saves all projects\n";
}
bool Save::execute(std::vector<std::string> args){

    if(std::shared_ptr<Settings> set = settings.lock()){
        int savingErrors { 0 };
        if(std::shared_ptr<FileIOManager> _fileManager = fileManager.lock()){
            if(std::shared_ptr<ProjectManager> _projectManager = projectManager.lock()){
                std::vector<std::shared_ptr<const Project>> projectsBuffer {_projectManager->getAllProjects()};
                for(auto proj : projectsBuffer ){
                    const Project& projectBuffer { *proj.get()};
                    if(!_fileManager->writeProject(projectBuffer, set->getProjectDirectory()))
                        ++savingErrors;
                }

                if(savingErrors != 0){
                    std::cout << "\tThere was an error saving " << savingErrors << " project(s)\n";
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
    else{
        std::cout << "\tNo settings object\n";
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
    this->description = "<arg>\n--total-time print total time of selected project\n--settings print settings\n--is-running for if a timer is running\n--time to show current clock time\n--runtime for duration of current timer\n";
}
bool Print::execute(std::vector<std::string> args){
    if(args.empty()){
        std::cout << "\tThis command requires one argument.\n";
        return false;
    }
    else if(args[0] == "--total-time"){
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
    else if(args[0] == "--settings"){
        if(std::shared_ptr<Settings> set = settings.lock()){
            std::cout << "\tProject Directory: \"" << set->getProjectDirectory() << "\"\n";
            std::cout << "\tHour Offset: " << set->getHourOffset() << "\n";
            std::cout << "\tVerbose Mode: " << std::boolalpha << set->getVerbose() << "\n";
            return true;
        }
        else
            std::cout << "\tThere is no settings object\n";
        return false;
    }
    else if(args[0] == "--is-running"){
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
    else if(args[0] == "--time"){
        Timestamp now;
        now.stamp();
        std::cout << "\t" << now.printTime() << " " << now.printDate() << std::endl;
        return true;
    }
    else if(args[0] == "--runtime"){
        if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
            if(manager->getProject()){
                if(manager->getProject()->getRunningTimerStartTime()){
                    Timestamp now;
                    now.stamp();

                    Timer duration { manager->getProject()->getRunningTimerStartTime()->getRawTime()};
                    duration.end(now.getRawTime());

                    std::cout << "\tRuntime for timer is " << duration.printDuration() << "\n";
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
    else{
        std::cout << "\tInvalid argument.\n";
        return false;
    }
}
Set::Set(std::string command,  std::weak_ptr<Settings> setSettings)
:   Command{command}, settings{setSettings}
{
    this->description = "<setting> <value>\n--hour-offset <int> Sets the clock offset\n--verbose <bool> True or false to set the mode of the application,\n--project-directory <string> Sets the project directory\n";
}
bool Set::execute(std::vector<std::string> args){
    if(std::shared_ptr<Settings> set = settings.lock()){
        if(args.size() > 2){
            std::cout << "\tThis command requires two arguments.\n";
            return false;
        }
        else if(args[0] == "--verbose"){
            if (args[1] == "true" || args[1] == "TRUE"){
                set->setVerbose(true);
                return true;
            }
            else if (args[1] == "false" || args[1] == "FALSE"){
                set->setVerbose(false);
                return true;
            }
            else{
                std::cout << "\tInvalid argument.\n";
                return false;
            }

        }
        else if(args[0] == "--hour-offset" && args.size() > 1){
            try{
                int offset {std::stoi(args[1])};
                set->setHourOffset(offset);
                Timestamp::setHourOffset(offset);
                return true;
            }
            catch(std::exception& e){
                std::cout << "\tInvalid argument.\n";
                return false;
            }
        }
        else if(args[0] == "--project-directory"){
            if(set->setProjectDirectory(args[1])){
                return true;
            }
            else{
                std::cout << "\tCould not set project save directory to \"" << args[1] << "\"\n";
                return false;
            }
        }
        else{
            std::cout << "\tInvalid argument \"" << args[0] << "\"\n";
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
bool ClearScreen::execute(std::vector<std::string> args){
    #ifdef __linux__
    system("clear -x");
    #endif

    #ifdef _Win32
    system("cls");
    #endif

    return true;
}