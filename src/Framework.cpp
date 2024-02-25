#include "Framework.h"

using json = nlohmann::json;

Framework::Framework(std::shared_ptr<ProjectManager> manager1, std::shared_ptr<FileIOManager> manager2,  std::shared_ptr<Settings> setSettings)
    : projectManager{manager1}, fileManager{manager2}, settings{setSettings}
{
    Timestamp::setHourOffset(settings->getHourOffset());
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
    if(settings){
        settings->readSettingsFile();

        fileManager->setDirectory(settings->getProjectDirectory());
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
    std::unique_ptr<PrintSettings>          printSettings   { new PrintSettings("print-settings", settings)};


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
    addCommand(std::move(printSettings));


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
