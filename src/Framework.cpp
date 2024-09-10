#include "Framework.h"

using json = nlohmann::json;
using namespace TimeTracker;

Framework::Framework(   std::shared_ptr<ProjectManager> setProjectManager, 
                        std::shared_ptr<FileIOManager> setFileManager, 
                        std::string setSettingsPath, 
                        std::string setDefaultProjectDirectory
                    )
                    :   projectManager{setProjectManager}, 
                        fileManager{setFileManager}, 
                        settingsPath{setSettingsPath}
{
    settings = std::make_shared<Settings>(Settings {setDefaultProjectDirectory});

}
Framework::~Framework(){
    
}
// MARK: Run
bool Framework::run(){

    bool exit { false };
    while(!exit){

        // Save every minute
        if(settings->getIsAutoSaveOn()){
            std::jthread savingThread(saveEveryMinute, projectManager, fileManager, settings);
            getInput(savingThread.get_stop_source());   // Handle commands

            savingThread.join();                        // After input is found, join threads
        }
        else
            getInput();

        switch (handleCommandQueue()) {
        case 1:
            exit = true;
            break;
        default:
            // Continue
            break;
        }
    }

    return true;
}
// MARK: Setup
bool Framework::setup(){

    setupCommands();
    
    readRCFile(settingsPath);
    handleCommandQueue();

    // Iterate through directory and populate project manager
    try{
        std::vector<ProjectPtr> projectBuffer { fileManager->readDirectory(settings->getProjectDirectory())};
        for(auto& proj : projectBuffer){
            projectManager->addProject(proj);
        }
    }
    catch (const std::invalid_argument& e){
        std::cerr << e.what();
        return false;
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
// MARK: PRIVATE FUNCTIONS
void Framework::getInput(std::stop_source savingThread){
    bool inputAccepted { false };
    
    // Query input
    std::string input;
    while (!inputAccepted){
        std::cout << "TIME TRACKER>";

        std::getline(std::cin, input);

        if(!input.empty() && !input.starts_with('\t'))
            inputAccepted = true;
    }
    commandQueue.push(input);

    savingThread.request_stop();
}
void Framework::getInput(){
    bool inputAccepted { false };
    
    // Query input
    std::string input;
    while (!inputAccepted){
        std::cout << "TIME TRACKER>";

        std::getline(std::cin, input);

        if(!input.empty() && !input.starts_with('\t'))
            inputAccepted = true;
    }
    commandQueue.push(input);
}
int Framework::handleCommandQueue(){
    for (/*Nothing*/; !commandQueue.empty(); commandQueue.pop()){
        std::vector<std::string>    arguments       { };
        std::stringstream           inputStream     { };
        
        inputStream << commandQueue.front();                   // Read the string
        
        // Parse the line
        std::string buffer;
        while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
            arguments.push_back(buffer);                // Add the quoted token or separated by space token
        }

        // Help command
        if(arguments[0] == "help"){
            std::cout << "Time Tracker help information for all commands.\n";
            std::cout << std::format("\t{:20}\t{}", "exit", "Exits the application.\n");

            // Iterate through commands list and print command and description
            for(auto i{commands.begin()}; i != commands.end(); ++i){

                // Buffer the output of the description accordingly to add padding to newlines
                std::string padding {"                                "};     // 32 spaces
                std::string formattedDescription {i->second->getDescription()};
                size_t charNum { 0 };
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
            commandQueue.pop();
            return 0;
        }
        else if(arguments[0] == "exit" || arguments[0] == "quit"){
            if(confirmExit())
                return 1;
            else
                return 0;
        }
        
        // Initial lookup of first command only
        if(commands.contains(arguments[0])){
            std::string command {arguments[0]};
            arguments.erase(arguments.begin()); // Remove command from arguments
            commands.at(command)->execute(arguments);
        }
        else
            std::cout << "\"" << arguments[0] << "\" is not a valid command.\n"; 
    }
    return 0;
}
void Framework::setupCommands(){
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
    std::unique_ptr<Reload>                 reload          { new Reload                {"reload",              projectManager, fileManager, settings}};
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
    addCommand(std::move(reload));
    addCommand(std::move(set));
    addCommand(std::move(clearScreen));
}
bool Framework::confirmExit(){
    ProjectPtr selectedProj {projectManager->getSelectedProject()};

    if(!selectedProj)
        return true;

    if(!selectedProj->isTimerRunning())
        return true;

    // Confirm exit if timer is running
    std::cout << "\tThere is a running timer for \"" << selectedProj->getName() << "\".\n\t"
            << "Do you wish to exit without saving? [y/N]>";

    std::string exitInput;
    std::getline(std::cin, exitInput);

    if(exitInput == "y" || exitInput == "Y")
        return true;
    else
        return false;
  
}   

void Framework::readRCFile(const std::string& path){
    if(std::filesystem::exists(path)){
        std::ifstream settingsFile {path}; 
        std::string line;
        while (std::getline(settingsFile, line))
        {
            const auto beginningOfString {line.find_first_not_of(" \t")};
            if (beginningOfString != std::string::npos){
                std::string trimmedLine {line.substr(beginningOfString)};
                if(!std::string{trimmedLine}.starts_with('#')){
                    commandQueue.push(trimmedLine);
                }   
            } 
        }
    }
    else{
        std::ofstream newSettings {path, std::ios::trunc};
        newSettings << "# Time Tracker Configuration File\n# Any commands entered here will be executed upon each startup" << std::endl;
        newSettings.close();
    }
}

// MARK: TIME TRACKER HELPER
void TimeTracker::saveEveryMinute(  std::stop_token stopSaving,  
                                    std::shared_ptr<ProjectManager> projectManager, 
                                    std::shared_ptr<FileIOManager>  fileManager,
                                    std::shared_ptr<Settings>       settings
                                )
{
    using namespace std::chrono_literals;
    bool continueSaving { true };

    if(!projectManager || !fileManager || !settings)
        return;

    uint32_t msPassed { 0 };
    while(continueSaving){
        if(msPassed >= 60000){  // 60,000ms = 1min.
            msPassed = 0;

            // Ensure there is a selected project AND a timer running
            if(projectManager->getSelectedProject() && projectManager->getSelectedProject()->isTimerRunning()){
                Project& project {*projectManager->getSelectedProject()};

                std::shared_ptr<Timestamp> timerStart {project.getRunningTimerStartTime()}; // Save running timer start time
                project.endTimer();

                fileManager->writeProject(settings->getProjectDirectory(), project);        // Write project to disk

                // Find the last timer and delete it so it doesnt keep adding timers                           
                auto& entries {project.getEntries()};
                std::string entryName;
                for(const auto& e : entries){
                    if(e->getRawStartTime() == timerStart->getRawTime()){
                        entryName = e->getName();
                        project.removeEntry(e->getID());
                        break;
                    }
                }

                // Start a new timer with the old start time
                project.startTimer(timerStart->getRawTime(), entryName);
            }
            
        }

        // Checks every 10ms if a stop is requested, if so it exits
        // Have to do this, if I slept for 1 minute, response time to
        // handle command would wait until this thread exited after the 1 min sleep
        if(stopSaving.stop_requested())
            continueSaving = false;
        else{
            std::this_thread::sleep_for(10ms);
            msPassed += 10;
        }
    }

    return;
}
std::string TimeTracker::determineSaveDirectory(){
    std::string projectsDirectory   {};
    std::string rcPath              {};

    #if defined __linux__

        #if defined DEBUG
            projectsDirectory   = "/mnt/NAS/1-Project-Related/Project-Source-Directories/Time-Tracker/Test-Directory/";
            rcPath              = projectsDirectory + ".timetracker-rc";
        #elif defined RELEASE
            std::string home {getenv("HOME")};
            rcPath = home + "/.timetracker-rc";
            projectsDirectory = home + "/Documents/Time-Tracker/";
        #else
            projectsDirectory = "";
            rcPath = "";
        #endif
	#elif defined _Win32
        projectsDirectory = "";
        rcPath = "";
	#else
        projectsDirectory = "";
        rcPath = "";
    #endif
    return projectsDirectory;
}

std::string TimeTracker::determineRCPath(){
    std::string projectsDirectory   {};
    std::string rcPath              {};

    #if defined __linux__

        #if defined DEBUG
            projectsDirectory   = "/mnt/NAS/1-Project-Related/Project-Source-Directories/Time-Tracker/Test-Directory/";
            rcPath              = projectsDirectory + ".timetracker-rc";
        #elif defined RELEASE
            std::string home {getenv("HOME")};
            rcPath = home + "/.timetracker-rc";
            projectsDirectory = home + "/Documents/Time-Tracker/";
        #else
            projectsDirectory = "";
            rcPath = "";
        #endif
	#elif defined _Win32
        projectsDirectory = "";
        rcPath = "";
	#else
        projectsDirectory = "";
        rcPath = "";
    #endif

    return rcPath;
}