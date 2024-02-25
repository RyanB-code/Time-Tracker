#include "Framework.h"

using json = nlohmann::json;

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


Settings::Settings(std::string setProjectDirectory, std::string setSettingsPath, bool setVerbose, int setHourOffset)
    :   settingsPath { setSettingsPath },
        projectDirectory {setProjectDirectory },
        verbose { setVerbose },
        hourOffset { setHourOffset }
{

}
Settings::~Settings(){

}
void Settings::setVerbose(bool set){
    verbose = set;
}
void Settings::setHourOffset(int set){
    hourOffset = set;
}
bool Settings::setProjectDirectory(std::string set){

    // Ensure backslash
	if(set.back() != '\\' && set.back() != '/'){
		set += "/";
	}

    // If it exists, set member variable
    if(std::filesystem::exists(set)){
        projectDirectory = set;
        return true;
    }
    else{

        // Create directory if it does not exist
        if(std::filesystem::create_directory(set)){
            projectDirectory = set;
            return true;
        }
        else // Failed to create directory
            return false;
    }

    return false;
}
bool Settings::setSettingsPath(std::string set){

    // If file exists, set member variable
    if(std::filesystem::exists(set)){
        settingsPath = set;
        return true;
    }
    else{
        std::ofstream settingsFile {set};       // Create file

        // Check if it exists again
        if(std::filesystem::exists(set)){
            settingsPath = set;
            return true;
        }
        else
            return false;
    }
    return false;
}
bool Settings::getVerbose() const{
    return verbose;
}
int Settings::getHourOffset() const{
    return hourOffset;
}
std::string Settings::getProjectDirectory() const{
    return projectDirectory;
}
std::string Settings::getSettingsPath() const{
    return settingsPath;
}
bool Settings::readSettingsFile(){
    std::ifstream inputFile{ settingsPath, std::ios::in };
    json j = json::parse(inputFile);

    bool verboseBuffer;
    int hourOffsetBuffer;
    std::string projectDirectoryBuffer, settingsPathBuffer;

    j.at("verbose").get_to(verboseBuffer);
	j.at("hourOffset").get_to(hourOffsetBuffer);
    j.at("projectDirectory").get_to(projectDirectoryBuffer);
    j.at("settingsPath").get_to(settingsPathBuffer);

    setVerbose(verboseBuffer);
    setHourOffset(hourOffsetBuffer);
    
    if(!setProjectDirectory(projectDirectoryBuffer) || !setSettingsPath(settingsPathBuffer)){
        return false;
    }
    else
        return true;
    
    return false;
}
bool Settings::writeSettingsFile() const{
    json j{
        {"verbose", verbose},
        {"hourOffset", hourOffset},
        ("projectDirectory", projectDirectory),
        {"settingsPath", settingsPath}
    };

    // Write to file here
    std::ofstream file{settingsPath};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
}
