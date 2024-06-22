#include "Framework.h"

using json = nlohmann::json;

Framework::Framework(std::shared_ptr<ProjectManager> manager1, std::shared_ptr<FileIOManager> manager2, std::string setSettingsPath, std::string defaultProjectDirectory)
    : projectManager{manager1}, fileManager{manager2}, settingsPath{setSettingsPath}
{
    settings = std::make_shared<Settings>(Settings {defaultProjectDirectory});

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

        if(input == "exit" || input == "quit"){
            // Check if there is a running timer
            ProjectPtr selectedProj {projectManager->getProject()};
            if(selectedProj){
                if(selectedProj->isTimerRunning()){
                    std::cout << "\tThere is a running timer for \"" << selectedProj->getName() << "\".\n\t"
                          << "Do you wish to exit without saving? [y/N]>";
                    std::string exitInput;
                    std::getline(std::cin, exitInput);

                    if(exitInput == "y" || exitInput == "Y"){
                        exit = true;
                        break;
                    }
                    else
                        input = ""; // Continue as normal
                }
                else{
                    exit = true;
                    break;
                }
            }
            else{
                exit = true;
                break;
            }
        }

        // Argument Buffers
        std::vector<std::string> arguments;
        std::stringstream inputStream{};
        
        inputStream << input;
        std::string buffer;
        while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
            arguments.push_back(buffer);                // Add to argument list
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

     // Add commands
    std::unique_ptr<DeselectProject>        deselectProject { new DeselectProject       {"desel",               projectManager}};
    std::unique_ptr<SelectProject>          selectProject   { new SelectProject         {"sel",                 projectManager}};
    std::unique_ptr<List>                   list            { new List                  {"ls",                  projectManager, settings}};
    std::unique_ptr<CreateProject>          createProject   { new CreateProject         {"mkproj",              projectManager}};
    std::unique_ptr<StartTimer>             startTimer      { new StartTimer            {"start",               projectManager}};
    std::unique_ptr<EndTimer>               endTimer        { new EndTimer              {"stop",                projectManager}};
    std::unique_ptr<DeleteProject>          deleteProject   { new DeleteProject         {"delproj",             projectManager, fileManager, settings}};
    std::unique_ptr<Print>                  print           { new Print                 {"print",               projectManager, fileManager, settings}};
    std::unique_ptr<Save>                   save            { new Save                  {"save",                projectManager, fileManager, settings}};
    std::unique_ptr<Set>                    set             { new Set                   {"set",                 settings}};
    std::unique_ptr<ClearScreen>            clearScreen     { new ClearScreen           {"clr"}};


    addCommand(std::move(deselectProject));
    addCommand(std::move(selectProject));
    addCommand(std::move(list));
    addCommand(std::move(createProject));
    addCommand(std::move(deleteProject));
    addCommand(std::move(startTimer));
    addCommand(std::move(endTimer));
    addCommand(std::move(print));
    addCommand(std::move(save));
    addCommand(std::move(set));
    addCommand(std::move(clearScreen));
    
    handleSettingsFile(settingsPath);

    // Iterate through directory and populate project manager
    std::vector<ProjectPtr> projectBuffer { fileManager->readDirectory(settings->getProjectDirectory())};
    if(!projectBuffer.empty()){
        for(auto& proj : projectBuffer){
            projectManager->addProject(proj);
        }
    }

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

            // Buffer the output of the description accordingly to add padding to newlines
            std::string padding {"                                "};     // 32 spaces
            std::string formattedDescription {i->second->getDescription()};
            int charNum { 0 };
            for(char& c : formattedDescription){
                ++charNum;
                if(c == '\n' && charNum != formattedDescription.size()){
                    formattedDescription.insert(charNum, padding);
                    charNum += padding.size();
                }
            }

            // Display output
            std::cout << std::format("\t{:20}\n", i->second->getCommand());
            std::cout << std::format("\t{:20}\t{}", " ", formattedDescription);
        }
        return;
    }
    // Initial lookup of first command only
    if(commands.contains(args[0])){
        std::string command {args[0]};
        args.erase(args.begin());
        commands.at(command)->execute(args);
    }
    else
        std::cout << "\t\"" << args[0] << "\" is not a valid command.\n"; 
}

void Framework::handleSettingsFile(std::string path){
    if(std::filesystem::exists(path)){
        std::ifstream settingsFile {path}; 
        std::string line;
        while (std::getline(settingsFile, line))
        {
            std::vector<std::string> arguments;             // Parsed line

            std::stringstream inputStream{};
            inputStream << line;
            std::string buffer;
            while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
                arguments.push_back(buffer);                // Add to argument list
            }

            if(!arguments.empty()){
                // Do not process # comment lines
                if(!arguments[0].starts_with("#"))
                    handleArguments(arguments);             // Apply command
            }
        }
    }
    else{
        std::ofstream newSettings {path, std::ios::trunc};
        newSettings << "# Time Tracker Configuration File\n# Any commands entered here will be executed upon each startup" << std::endl;
        newSettings.close();
    }
}
