#include "Commands.h"

using namespace TimeTracker;

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


ProjectCommand::ProjectCommand(std::string command, std::weak_ptr<ProjectManager> setProjectManager) 
    : Command{command}, projectManager {setProjectManager}
{

}
ProjectCommand::~ProjectCommand(){

}
// MARK: Select Project
SelectProject::SelectProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager)
    : ProjectCommand{command, setProjectManager}
{
    this->description = "<string> Selects a project by name.\n";
}
bool SelectProject::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager = projectManager.lock();

    if(!tempProjectManager)
        return false;
    
    if(!tempProjectManager->selectProject(args[0])){
        std::cerr << "\tNo project named \"" << args[0] << "\" exists.\n";
        return false;
    }

    return true;
}
// MARK: Deselect Project
DeselectProject::DeselectProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager)
    : ProjectCommand{command, setProjectManager}
{
    this->description = "Deselects the current project.\n";
}
bool DeselectProject::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager = projectManager.lock();

    if(!tempProjectManager)
        return false;
    
    tempProjectManager->deselectProject();
    return true;
}
// MARK: List
List::List( std::string command, 
            std::weak_ptr<ProjectManager> setProjectManager, 
            std::weak_ptr<Settings> setSettings
        )
        :   ProjectCommand(command, setProjectManager), 
            settings{setSettings}
{
    this->description = "<arg>\n-p Lists projects\n-e Lists entries of selected project.\n";
}
bool List::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager  { projectManager.lock() };
    std::shared_ptr<Settings>       tempSettings        { settings.lock() };

    if(!tempProjectManager || !tempSettings)
        return false;

   
    // List all project names
    if(args[0] == "-p"){
        // Ensure map is not empty
        if(tempProjectManager->getAllProjectNames().empty()){
            std::cout << "\tThere are no tracked projects.\n";
            return true;
        }
        else{
            std::cout << "Current Projects:\n";
            for(std::string& s : tempProjectManager->getAllProjectNames()){
                std::cout << '\t' << s << "\n";
            }
            return true;
        }
    }
    else if(args[0] == "-e"){
        if(!tempProjectManager->getSelectedProject()){
            std::cout << "\tThere is no project selected.\n";
            return false;
        }

        // Indent for every newline
        std::string formatted {tempProjectManager->getSelectedProject()->printAllEntries(tempSettings->getEntryNameWidth()).str()};
        size_t charNum { 0 };
        for(char& c : formatted){
            ++charNum;
            if(c == '\n' && charNum < formatted.size())
                formatted.insert(charNum, "\t");
        }
        std::cout << "\t" << formatted;
        return true;
    }
    else{
        std::cerr << "\tInvalid argument.\n";
        return false;
    }
}
// MARK: Create Project
CreateProject::CreateProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager)
    : ProjectCommand{command, setProjectManager}
{
    this->description = "<string> Creates a new project with the given name and automatically selects it.\n";
}
bool CreateProject::execute(const std::vector<std::string>& args){
    if(args.empty()){
        std::cerr << "\tThis command requires one argument\n";
        return false;
    }

   std::shared_ptr<ProjectManager> tempProjectManager = projectManager.lock();

    if(!tempProjectManager)
        return false;
    
    ProjectPtr newProject{ new Project (args[0])};

    if(!tempProjectManager->addProject(newProject)){
         std::cerr << "\tCould not create project with name \"" << args[0] << "\". \n\tIt may already exist.\n";
        return false;
    }
        
    tempProjectManager->selectProject(args[0]);
    return true;
}
// MARK: Delete Project
DeleteProject::DeleteProject(   std::string command,
                                std::weak_ptr<ProjectManager>   setProjectManager, 
                                std::weak_ptr<FileIOManager>    setFileManager, 
                                std::weak_ptr<Settings>         setSettings
                            ) : 
                                ProjectCommand  {command, setProjectManager}, 
                                fileManager     {setFileManager}, 
                                settings        { setSettings }
{
    this->description = "[name] Deletes the selected project. If no project is selected, then it will delete the project with the given name.\n";
}
bool DeleteProject::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager  { projectManager.lock() };
    std::shared_ptr<Settings>       tempSettings        { settings.lock() };
    std::shared_ptr<FileIOManager>  tempFileManager     { fileManager.lock() }; 

    if(!tempProjectManager || !tempSettings || !tempFileManager)
        return false;


    std::string projectName;

    // Find proj name from selected project if no arg given
    if(args.empty()){
        if(!tempProjectManager->getSelectedProject()){
            std::cerr << "\tThere is no project selected.\n";
            return false;
        }

        projectName = tempProjectManager->getSelectedProject()->getName(); 
    }
    // Find project name from argument
    else{
        std::string find {args[0]};
        if(!tempProjectManager->findProject(find)){
            std::cout << "\tCould not find project \"" << find << "\"\n";
            return false;
        }

        projectName = find;
    }

    // Find fully qualifed path
    std::string fullProjectPath { tempSettings->getProjectDirectory()};
    fullProjectPath += projectName;
    fullProjectPath += ".json";

    if(!tempProjectManager->deleteProject(projectName)){
        std::cerr << "\tError deleting project \"" << projectName << "\"\n";
        return false;
    }

    if(!tempFileManager->deleteProject(fullProjectPath)){
        std::cerr << "\tError deleting project file \"" << fullProjectPath << "\"\n";
        return false;
    }

    return true;
}
// MARK: Reload
Reload::Reload( std::string command, 
                std::weak_ptr<ProjectManager>   setProjectManager, 
                std::weak_ptr<FileIOManager>    setFileManager,
                std::weak_ptr<Settings>         setSettings
            )
            :   ProjectCommand  { command, setProjectManager },
                fileManager     { setFileManager },
                settings        { setSettings }

{
    this->description = "Reloads projects from project directory\n";

}
Reload::~Reload(){

}
bool Reload::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager  = projectManager.lock();
    std::shared_ptr<FileIOManager>  tempFileManager     = fileManager.lock();
    std::shared_ptr<Settings>       tempSettings        = settings.lock();


    if(!tempProjectManager || !tempFileManager || !tempSettings)
        return false;


    tempProjectManager->deselectProject();
    tempProjectManager->clearProjects();

    // Iterate through directory and populate project manager
    try{
        std::vector<ProjectPtr> projectBuffer { tempFileManager->readDirectory(tempSettings->getProjectDirectory())};
        for(auto& proj : projectBuffer){
            tempProjectManager->addProject(proj);
        }
    }
    catch (const std::invalid_argument& e){
        std::cerr << e.what();
        return false;
    }

    return true;
}
// MARK: Start Timer
StartTimer::StartTimer(std::string command, std::weak_ptr<ProjectManager> setProjectManager)
    : ProjectCommand{command, setProjectManager}
{
    this->description = "[string] Starts a new timer for the selected project with a given name, if one is provided.\n";
}
bool StartTimer::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager = projectManager.lock();

    if(!tempProjectManager)
        return false;
    
    if(!tempProjectManager->getSelectedProject()){
        std::cout << "\tThere is no project selected.\n";
        return false;
    }

    if(args.empty())
        if(!tempProjectManager->getSelectedProject()->startTimer()){
            std::cerr << "\tCould not start timer. There may be a timer already running.\n";
            return false;
        }
        else
            return true;
    else{
        if(!tempProjectManager->getSelectedProject()->startTimer(args[0])){
            std::cerr << "\tCould not start timer. There may be a timer already running.\n";
            return false;
        }
        else
            return true;
    }
}
// MARK: End Timer
EndTimer::EndTimer(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand{command, manager}
{
    this->description = "Ends a running timer for the selected project.\n";
}
bool EndTimer::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager = projectManager.lock();

    if(!tempProjectManager)
        return false;
    
    if(!tempProjectManager->getSelectedProject()){
        std::cout << "\tThere is no project selected.\n";
        return false;
    }

    if(!tempProjectManager->getSelectedProject()->endTimer()){
        std::cout << "\tThere is no running timer for the selected project.\n";
        return false;
    }
    
    return true;
}
// MARK: Save
Save::Save( std::string                     command, 
            std::weak_ptr<ProjectManager>   setProjectManager, 
            std::weak_ptr<FileIOManager>    setFileManager, 
            std::weak_ptr<Settings>         setSettings
        ) :   
            Command         {command}, 
            projectManager  {setProjectManager}, 
            fileManager     {setFileManager}, 
            settings        {setSettings}
{
    this->description = "Saves all projects\n";
}
bool Save::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager  { projectManager.lock() };
    std::shared_ptr<Settings>       tempSettings        { settings.lock() };
    std::shared_ptr<FileIOManager>  tempFileManager     { fileManager.lock() }; 

    if(!tempProjectManager || !tempSettings || !tempFileManager)
        return false;

    int savingErrors { 0 };
    std::vector<std::shared_ptr<const Project>> projectsBuffer {tempProjectManager->getAllProjects()};
    for(auto proj : projectsBuffer ){
        const Project& projectBuffer { *proj.get()};
        if(!tempFileManager->writeProject(tempSettings->getProjectDirectory(), projectBuffer))
            ++savingErrors;
    }

    if(savingErrors != 0){
        std::cerr << "\tThere was an error saving " << savingErrors << " project(s)\n";
        return false;
    }
    else{
        return true;
    }
}


Print::Print(   std::string                     command, 
                std::weak_ptr<ProjectManager>   setProjectManager, 
                std::weak_ptr<FileIOManager>    setFileManager, 
                std::weak_ptr<Settings>         setSettings
            ) :   
                Command         {command}, 
                projectManager  {setProjectManager}, 
                fileManager     {setFileManager}, 
                settings        {setSettings}
{
    this->description = "<arg>\n--version \tPrints version information\n"
                                "--total-time \tPrint total time of selected project\n"
                                "--settings \tPrint settings\n"
                                "--is-running \tShow if a timer is running\n"
                                "--time \t\tShows current clock time\n"
                                "--runtime \tShows duration of current timer\n";
}
bool Print::execute(const std::vector<std::string>& args){

    std::shared_ptr<ProjectManager> tempProjectManager  { projectManager.lock() };
    std::shared_ptr<Settings>       tempSettings        { settings.lock() };
    std::shared_ptr<FileIOManager>  tempFileManager     { fileManager.lock() }; 

    if(!tempProjectManager || !tempSettings || !tempFileManager)
        return false;


    if(args.empty()){
        std::cerr << "\tThis command requires one argument.\n";
        return false;
    }
    else if(args[0] == "--total-time"){
        if(tempProjectManager->getSelectedProject()){
            std::cout << "\tTotal time for this project is: " << tempProjectManager->getSelectedProject()->printTotalTime().substr(0,8) <<"  [HH:MM:SS]\n";
            return true;
        }
        else{
            std::cerr << "\tThere is no project selected.\n";
            return false;
        }   
    }
    else if(args[0] == "--settings"){
        std::cout << "\tProject Directory: \""              << tempSettings->getProjectDirectory() << "\"\n";
        std::cout << "\tVerbose Mode: " << std::boolalpha   << tempSettings->getVerbose() << "\n";
        std::cout << "\tEntry Name Text Box Width: "        << (int)tempSettings->getEntryNameWidth() << "\n";
        return true;
    }
    else if(args[0] == "--is-running"){
        if(tempProjectManager->getSelectedProject()){
            if(tempProjectManager->getSelectedProject()->isTimerRunning()){
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
    else if(args[0] == "--time"){
        Timestamp now;
        now.stamp();
        std::cout << "\t" << now.printTime().substr(0, 8) << " " << now.printDate() << std::endl;
        return true;
    }
    else if(args[0] == "--runtime"){
        if(tempProjectManager->getSelectedProject()){
            if(tempProjectManager->getSelectedProject()->getRunningTimerStartTime()){
                Timer duration { tempProjectManager->getSelectedProject()->getRunningTimerStartTime()->getRawTime()};
                duration.end();

                std::cout << "\tRuntime for timer is " << duration.printDuration().substr(0, 8) << "\n";
                return true;
            }
            else{  
                std::cerr << "\tNo timer is running for the selected project.\n";
                return false;
            }
        }
        else{ 
            std::cerr << "\tThere is no project selected.\n";
            return false;
        }
    }
    else if(args[0] == "--version"){
        std::cout << TimeTracker::getAllProjectInfo() << "\n";
        return true;
    }
    else{ 
        std::cerr << "\tInvalid argument.\n";
        return false;
    }
}
// MARK: Set
Set::Set(std::string command,  std::weak_ptr<Settings> setSettings)
:   Command{command}, settings{setSettings}
{
    this->description = "<setting> <value>\n"
                        "--verbose <bool> \t\tTrue or false to set the mode of the application\n"
                        "--project-directory <string> \tSets the project directory\n"
                        "--entry-name-width <int> \tSets the width of the name of entries\n";
}
bool Set::execute(const std::vector<std::string>& args){
    std::shared_ptr<Settings> tempSettings { settings.lock() };

    if(!tempSettings)
        return false;


    if(args.size() > 2){
        std::cerr << "\tThis command requires two arguments.\n";
        return false;
    }
    else if(args[0] == "--verbose"){
        if (args[1] == "true" || args[1] == "TRUE"){
            tempSettings->setVerbose(true);
            return true;
        }
        else if (args[1] == "false" || args[1] == "FALSE"){
            tempSettings->setVerbose(false);
            return true;
        }
        else{
            std::cout << "\tInvalid argument.\n";
            return false;
        }

    }
    else if(args[0] == "--project-directory"){
        if(tempSettings->setProjectDirectory(args[1])){
            return true;
        }
        else{
            std::cerr << "\tCould not set project save directory to \"" << args[1] << "\"\n";
            return false;
        }
    }
    else if(args[0] == "--entry-name-width"){
        try{
            int width {std::stoi(args[1])};
            tempSettings->setEntryNameWidth(width);
            return true;
        }
        catch(std::exception& e){
            std::cerr << "\tInvalid argument.\n";
            return false;
        }
    }
    else{
        std::cerr << "\tInvalid argument \"" << args[0] << "\"\n";
        return false;
    }
}
// MARK: Clear Screen
ClearScreen::ClearScreen(std::string command) : Command{command} {
    this->description = "Clears the screen.\n";
}
bool ClearScreen::execute(const std::vector<std::string>& args){
    #ifdef __linux__
    system("clear -x");
    #endif

    #ifdef _Win32
    system("cls");
    #endif

    return true;
}
