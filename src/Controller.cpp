#include "Controller.h"



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

Framework::Framework(std::shared_ptr<ProjectManager> manager1, std::shared_ptr<FileIOManager> manager2, int hourOffset)
    : projectManager{manager1}, fileManager{manager2}
{
    Timestamp::setHourOffset(hourOffset);
}
Framework::~Framework(){
    
}
bool Framework::run(){
    setup();

    bool exit {false};
    while(!exit){
        std::cout << "TIME TRACKER>";

        // Query input
        std::string input;
        std::getline(std::cin, input);

        if(input == "exit"){
            exit = true;
            break;
        }

        // Argument Buffers
        std::vector<std::string> arguments;
        std::stringstream inputStream{};
        

        // Add to argument list
        inputStream << input;
        std::string buffer;
        while (inputStream >> std::quoted(buffer)) {
            arguments.push_back(buffer);
        }

        // Handle commands
        handleArguments(arguments);
    }

    return true;
}
bool Framework::addCommand(std::unique_ptr<Command> command){
    if(commands.contains(command->getCommand())){
        return false;
    }
    else{
        commands.try_emplace(command->getCommand(), std::move(command));
        return true;
    }
    return false;
}
bool Framework::setup(){

    // Iterate through directory and populate project manager
    std::vector<ProjectPtr> projectBuffer { fileManager->readDirectory()};
    if(!projectBuffer.empty()){
        for(auto& proj : projectBuffer){
            projectManager->addProject(proj);
        }
    }


    // Add commands
    std::unique_ptr<DeselectProject>        deselectProject { new DeselectProject{"deselect", projectManager}};
    std::unique_ptr<SelectProject>          selectProject   { new SelectProject{"select", projectManager}};
    std::unique_ptr<ListProjects>           listProjects    { new ListProjects{"list", projectManager}};
    std::unique_ptr<ListEntries>            listEntries     { new ListEntries("list-entries", projectManager)};
    std::unique_ptr<CreateProject>          createProject   { new CreateProject("create", projectManager)};
    std::unique_ptr<DeleteProject>          deleteProject   { new DeleteProject("delete", projectManager, fileManager)};
    std::unique_ptr<StartTimer>             startTimer      { new StartTimer("start", projectManager)};
    std::unique_ptr<EndTimer>               endTimer        { new EndTimer("end", projectManager)};
    std::unique_ptr<TotalTime>              totalTime       { new TotalTime("total-time", projectManager)};
    std::unique_ptr<IsRunning>              isRunning       { new IsRunning("is-running", projectManager)};
    std::unique_ptr<SaveAllProjects>        saveAllProjects { new SaveAllProjects("save-all", fileManager, projectManager)};
    std::unique_ptr<PrintFileIODirectory>   printDirectory  { new PrintFileIODirectory("print-file-directory", fileManager)};

    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));
    addCommand(std::move(listProjects));
    addCommand(std::move(listEntries));
    addCommand(std::move(createProject));
    addCommand(std::move(deleteProject));
    addCommand(std::move(startTimer));
    addCommand(std::move(endTimer));
    addCommand(std::move(totalTime));
    addCommand(std::move(isRunning));
    addCommand(std::move(saveAllProjects));
    addCommand(std::move(printDirectory));


    return true;
}
void Framework::handleArguments(std::vector<std::string>& args){
    // Ensure not empty
    if(args.empty()){
        return;
    }

    // Help command
    if(args[0] == "help"){
        std::cout << "Time Tracker help information for all commands.\n";
        for(auto i{commands.begin()}; i != commands.end(); ++i){
            std::cout << std::format("\t{:20}\t{}", i->second->getCommand(), i->second->getDescription());
        }
        return;
    }

    // Initial lookup of first command only
    if(commands.contains(args[0])){
        // If command exists, then pass the next argument
        // Currently only handles 1 argument
        if(args.size() > 1) 
            commands.at(args[0])->execute(args[1]);
        else    
            commands.at(args[0])->execute();

    }
    else
        std::cout << "\t\"" << args[0] << "\" is not a valid command.\n"; 
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
    this->description = "Selects a project by name.\n";
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
    this->description = "Removes any selected project.\n";
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
    this->description = "Lists all tracked projects by name.\n";
}
bool ListProjects::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){

        // Ensure it is not empty
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
    else
        return false;
}
ListEntries::ListEntries(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Lists all entries of the selected project.\n";
}
bool ListEntries::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject() ){
            std::cout << manager->getProject()->printAllEntries().str();
            return true;
        }
        else{
            std::cout << "\tThere is no project selected.\n";
            return false;
        }
    }
    else
        return false;

    return false;
}
CreateProject::CreateProject(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Creates a new project with the a given name and automatically selects it.\n";
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
    this->description = "Deletes the selected project. If no project is selected, then it will delete the project with the given name.\n";
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
            std::string fullProjectPath { manager2->getDirectory()};
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
    this->description = "Starts a new timer for the selected project with a given name, if one is provided.\n";
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
    this->description = "Check if a timer is running for the selected project, or of the project of the given name if no project is selected.\n";
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
TotalTime::TotalTime(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Views the total time of the selected project, or of the project of the given name if no project is selected.\n";
}
bool TotalTime::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject()){
            std::cout << "\tTotal time for this project is: " << manager->getProject()->printTotalTime() <<"  [HH:MM:SS]\n";
            return true;
        }
        else{
            if(arg.empty()){
                std::cout << "\tThis command requires either a project to be selected or a name to be provided.\n";
                return false;
            }
            else{
                if(manager->findProject(arg)){
                    std::cout << "\tTotal time for this project is: " << manager->findProject(arg)->printTotalTime() <<"  [HH:MM:SS]\n";
                    return true;
                }
                else{
                    std::cout << "\tCould not find project \"" << arg << "\"\n";
                    return false;
                }
            }
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

SaveAllProjects::SaveAllProjects(std::string command, std::weak_ptr<FileIOManager> fileManager, std::weak_ptr<ProjectManager> projectManager)
    :   FileIOCommand(command, fileManager), projectManager{projectManager}
{
    this->description = "Saves all projects to the specific working directory.\n";
}
bool SaveAllProjects::execute(std::string arg){
    if(std::shared_ptr<FileIOManager> _fileManager = fileManager.lock()){
    
        if(std::shared_ptr<ProjectManager> _projectManager = projectManager.lock()){

            std::vector<std::shared_ptr<const Project>> projectsBuffer {_projectManager->getAllProjects()};
            for(auto proj : projectsBuffer ){
                const Project& projectBuffer { *proj.get()};
                _fileManager->writeProject(projectBuffer);
            }
        }
    }
    else
        return false;

    return false;
}
PrintFileIODirectory::PrintFileIODirectory(std::string command, std::weak_ptr<FileIOManager> fileManager)
    :   FileIOCommand(command, fileManager)
{
    this->description = "Saves all projects to the specific working directory.\n";
}
bool PrintFileIODirectory::execute(std::string arg){
    if(std::shared_ptr<FileIOManager> manager = fileManager.lock()){
        std::cout << "\tFile IO Dreictory is \"" << manager->getDirectory() << "\"\n"; 
    }
    else
        return false;
    
    return false;
}