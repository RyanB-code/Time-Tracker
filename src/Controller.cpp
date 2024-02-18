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
    
        std::string input;
        std::cin >> input;

        if(commands.contains(input)){
            std::cout << "command found \n";
        }
        else if(input == "exit")
            exit = true;
        else
            std::cout << "command not found \n";
        
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
    std::unique_ptr<SelectProject>      selectProject { new SelectProject{"select", projectManager}};


    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));

    return true;
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
    : projectManager {manager}, Command(command) 
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
       return manager->selectProject(arg);
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



FileIOCommand::FileIOCommand(std::string command, std::unique_ptr<ProjectFormatter> format)
    : Command {command}
{
    saveFormat = std::move(format);
}
FileIOCommand::~FileIOCommand(){

}

