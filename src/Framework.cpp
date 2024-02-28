#include "Framework.h"

using json = nlohmann::json;

Framework::Framework(std::shared_ptr<ProjectManager> manager1, std::shared_ptr<FileIOManager> manager2)
    : projectManager{manager1}, fileManager{manager2}
{
    
}
Framework::~Framework(){
    
}
bool Framework::run(){

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

    // Read settings file
    if(!settings){
        settings = fileManager->readSettingsFile();
    }

    if(settings && fileManager){
        Timestamp::setHourOffset(settings->getHourOffset());
    }
    else{
        return false;
    }

    // Iterate through directory and populate project manager
    std::vector<ProjectPtr> projectBuffer { fileManager->readDirectory()};
    if(!projectBuffer.empty()){
        for(auto& proj : projectBuffer){
            projectManager->addProject(proj);
        }
    }


    // Add commands
    std::unique_ptr<DeselectProject>        deselectProject { new DeselectProject       {"desel",               projectManager}};
    std::unique_ptr<SelectProject>          selectProject   { new SelectProject         {"sel",                 projectManager}};
    std::unique_ptr<ListProjects>           listProjects    { new ListProjects          {"ls",                  projectManager}};
    std::unique_ptr<CreateProject>          createProject   { new CreateProject         {"mkproj",              projectManager}};
    std::unique_ptr<IsRunning>              isRunning       { new IsRunning             {"is-running",          projectManager}};
    std::unique_ptr<StartTimer>             startTimer      { new StartTimer            {"start",               projectManager}};
    std::unique_ptr<EndTimer>               endTimer        { new EndTimer              {"stop",                projectManager}};
    std::unique_ptr<DeleteProject>          deleteProject   { new DeleteProject         {"delproj",             projectManager, fileManager}};
    std::unique_ptr<Print>                  print           { new Print                 {"print",               projectManager, fileManager, settings}};
    std::unique_ptr<Save>                   save            { new Save                  {"save",                projectManager, fileManager, settings}};
    std::unique_ptr<RefreshSettings>        refreshSettings { new RefreshSettings       {"refresh-settings",    settings, fileManager}};
    std::unique_ptr<SetVerbose>             setVerbose      { new SetVerbose            {"set-verbose",         settings}};
    std::unique_ptr<SetHourOffset>          setHourOffset   { new SetHourOffset         {"set-hour-offset",     settings}};
    std::unique_ptr<SetProjectDirectory>    setProjDirec    { new SetProjectDirectory   {"set-proj-dir",        settings}};


    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));
    addCommand(std::move(listProjects));
    addCommand(std::move(createProject));
    addCommand(std::move(deleteProject));
    addCommand(std::move(startTimer));
    addCommand(std::move(endTimer));
    addCommand(std::move(print));
    addCommand(std::move(isRunning));
    addCommand(std::move(save));
    addCommand(std::move(refreshSettings));
    addCommand(std::move(setVerbose));
    addCommand(std::move(setHourOffset));
    addCommand(std::move(setProjDirec));


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
        std::cout << std::format("\t{:20}\t{}", "exit", "Exits the application.\n");

        // Iterate through commands list and print command and description
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
