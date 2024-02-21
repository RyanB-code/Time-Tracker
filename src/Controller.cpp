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

EventHandler::EventHandler(std::shared_ptr<ProjectManager> manager, std::shared_ptr<ProjectFormatter> format, int hourOffset)
    : projectManager{manager}, saveFormat{format}
{
    Timestamp::setHourOffset(hourOffset);
}
EventHandler::~EventHandler(){
    
}
bool EventHandler::run(){
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
bool EventHandler::addCommand(std::unique_ptr<Command> command){
    if(commands.contains(command->getCommand())){
        return false;
    }
    else{
        commands.try_emplace(command->getCommand(), std::move(command));
        return true;
    }
    return false;
}
bool EventHandler::setup(){
    std::unique_ptr<DeselectProject>    deselectProject { new DeselectProject{"deselect", projectManager}};
    std::unique_ptr<SelectProject>      selectProject   { new SelectProject{"select", projectManager}};
    std::unique_ptr<ListProjects>       listProjects    { new ListProjects{"list", projectManager}};
    std::unique_ptr<ReadFile>           readFile        { new ReadFile{"read", saveFormat, projectManager} };
    std::unique_ptr<ListEntries>        listEntries     { new ListEntries("list-entries", projectManager)};
    std::unique_ptr<CreateProject>      createProject   { new CreateProject("create", projectManager)};
    std::unique_ptr<DeleteProject>      deleteProject   { new DeleteProject("delete", projectManager)};
    std::unique_ptr<StartTimer>         startTimer      { new StartTimer("start", projectManager)};
    std::unique_ptr<EndTimer>           endTimer        { new EndTimer("end", projectManager)};
    std::unique_ptr<TotalTime>          totalTime       { new TotalTime("total-time", projectManager)};
    std::unique_ptr<IsRunning>          isRunning       { new IsRunning("is-running", projectManager)};


    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));
    addCommand(std::move(listProjects));
    addCommand(std::move(readFile));
    addCommand(std::move(listEntries));
    addCommand(std::move(createProject));
    addCommand(std::move(deleteProject));
    addCommand(std::move(startTimer));
    addCommand(std::move(endTimer));
    addCommand(std::move(totalTime));
    addCommand(std::move(isRunning));

    return true;
}
void EventHandler::handleArguments(std::vector<std::string>& args){
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
    this->description = "Creates a new project with the a given name.\n";
}
bool CreateProject::execute(std::string arg){
    if(arg.empty()){
        std::cout << "\tThis command requires one argument\n";
        return false;
    }

    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        ProjectPtr newProject{ new Project (arg)};

        if(manager->addProject(newProject)){
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
DeleteProject::DeleteProject(std::string command, std::weak_ptr<ProjectManager> manager)
    : ProjectCommand(command, manager)
{
    this->description = "Deletes the selected project. If no project is selected, then it will delete the project with the given name.\n";
}
bool DeleteProject::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){

        if(arg.empty()){
            if(manager->getProject() ){

                std::string name {manager->getProject()->getName()};

                if(manager->deleteProject(name)){
                    std::cout << "\tDeleted selected project \"" << name << "\"\n";
                    return true;
                }
                return true;
            }
            else{
                std::cout << "\tThere is no project selected.\n";
                return true;
            }
        }
        else{
            if(manager->deleteProject(arg)){
                std::cout << "\tDeleted project \"" << arg << "\"\n";
                return true;
            }
            else{
                std::cout << "\tCould not find project \"" << arg << "\"\n";
                return false;
            }
        }
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
                return manager->getProject()->startTimer();
            else
                return manager->getProject()->startTimer(arg);
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



FileIOCommand::FileIOCommand(std::string command, std::weak_ptr<ProjectFormatter> format)
    : Command {command}, saveFormat{format}
{

}
FileIOCommand::~FileIOCommand(){

}

ReadFile::ReadFile(std::string command, std::weak_ptr<ProjectFormatter> format, std::weak_ptr<ProjectManager> manager)
    :   FileIOCommand(command, format), projectManager{manager}
{
    this->description = "Reads a specific file for project information and adds to tracked projects.\n";
}
bool ReadFile::execute(std::string arg){
    if(std::shared_ptr<ProjectFormatter> format = saveFormat.lock()){
    
        if( std::shared_ptr<ProjectManager> manager = projectManager.lock()){

            if(std::filesystem::exists(arg)){
                ProjectPtr projBuffer { new Project {format->read(arg)}};
                return manager->addProject(projBuffer);
            }
            else{
                std::cout << "\tThe file \"" << arg << "\" could not be found.\n";
                return false;
            }
        }
    }
    else
        return false;

    return false;
}
