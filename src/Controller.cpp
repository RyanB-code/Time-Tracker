#include "Controller.h"

EventHandler::EventHandler(std::shared_ptr<ProjectManager> manager, std::shared_ptr<ProjectFormatter> format)
    : projectManager{manager}, saveFormat{format}
{

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


    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));
    addCommand(std::move(listProjects));
    addCommand(std::move(readFile));
    addCommand(std::move(listEntries));



    return true;
}
void EventHandler::handleArguments(std::vector<std::string>& args){
    // Ensure not empty
    if(args.empty()){
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


Command::Command(std::string text) : command{text}
{

}
Command::~Command(){

}
std::string Command::getCommand() {
    return command;
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

}
bool ListEntries::execute(std::string arg){
    if(std::shared_ptr<ProjectManager> manager = projectManager.lock() ){
        if(manager->getProject() ){
            std::cout << manager->getProject()->printAllEntries().str();
            return true;
        }
        else{
            std::cout << "\tThere is no selected project.\n";
            return true;
        }
    }
    else
        return false;

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
