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



        // Paging
        termios originalAttributes { CommandHelper::enableRawMode() }; // Saves current terminal settings before overriding
        try {
            bool waitForInput { true };
            char c;
            uint64_t    start      { 0 };
            uint64_t    interval   { tempSettings->getEntriesPerPage() };
            uint64_t    size       { tempProjectManager->getSelectedProject()->getEntriesSize()};
            bool        arrowKeyFound           { false };
            bool        leftSquareBracketFound  { false };
            bool        updatePage              { true };
            bool        firstTime               { true };
            int         previousLines           { 0 };
            while(waitForInput){

                if(updatePage){
                    // Print entries
                    std::string table {tempProjectManager->getSelectedProject()->printRecentEntryRange(start, interval, tempSettings->getEntryNameWidth()).str()};
                    size_t charNum  { 0 };
                    int lines       { 1 }; // Starts at 2 since there is always one line, and information lines at bottom
                    for(char& c : table){
                        ++charNum;
                        if(c == '\n')
                            ++lines;
                    }

                    std::ostringstream moveCursorUp;
                    if(previousLines == 0){
                        moveCursorUp << "\033[" << lines << "A";
                    }
                    else{
                        CommandHelper::clearRelativeTerminalSection(previousLines, previousLines);
                        moveCursorUp << "\033[" << previousLines << "A";
                    }

                    // Apply cursor
                    if(!firstTime)
                        std::cout << moveCursorUp.str();
                    else
                        firstTime = false;

                    previousLines = lines;
                    std::cout << table;
                    std::cout << "Use arrow keys to navigate pages. Press 'Q' to quit.\n";

                    updatePage = false;
                }   

                read (STDIN_FILENO, &c, 1);

                switch (c){
                    case 'Q':
                        waitForInput = false;
                        break;
                    case 'q':
                        waitForInput = false;
                        break;
                    case '\033':
                        arrowKeyFound = true;
                        break;
                    case '[':
                        leftSquareBracketFound = true;
                        break;
                    case 'C':
                        // Right Arrow
                        if(arrowKeyFound && leftSquareBracketFound){
                            updatePage = true;

                            // Increment pages
                            if( (start + interval) < size)
                                start += interval;
                            // Do not add to start if the size will be too large
                        }
                        
                        arrowKeyFound = false;
                        leftSquareBracketFound = false;
                        break;
                    case 'D':
                        // Left Arrow
                        if(arrowKeyFound && leftSquareBracketFound){
                           updatePage = true;
                            if( int(start - interval) < 0)
                                start = 0;
                            else
                                start -= interval;
                        }
                        
                        arrowKeyFound = false;
                        leftSquareBracketFound = false;
                        break;
                    default:
                        break;
                }
            }
            // Must call disable before returning
            CommandHelper::disableRawMode(originalAttributes);
        }
        catch (std::exception& e){
            std::cerr << e.what() << "\n";
            CommandHelper::disableRawMode(originalAttributes);

            return false;
        }
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
    this->description = "No arguments saves all projects.\n"
                        "[project name] saves the project with given name\n";
}
bool Save::execute(const std::vector<std::string>& args){
    std::shared_ptr<ProjectManager> tempProjectManager  { projectManager.lock() };
    std::shared_ptr<Settings>       tempSettings        { settings.lock() };
    std::shared_ptr<FileIOManager>  tempFileManager     { fileManager.lock() }; 

    if(!tempProjectManager || !tempSettings || !tempFileManager)
        return false;

    if(args.empty()){
        std::vector<std::string> savingErrors;
        std::vector<std::shared_ptr<const Project>> projectsBuffer {tempProjectManager->getAllProjects()};
        for(auto proj : projectsBuffer ){
            const Project& projectBuffer { *proj.get()};
            if(!tempFileManager->writeProject(tempSettings->getProjectDirectory(), projectBuffer))
                savingErrors.push_back(std::string{proj->getName()});   // Save name of the project that could not be saved
        }

        // Print projects that couldnt be saved
        if(!savingErrors.empty()){
            std::cerr << "\tThere was an error saving " << savingErrors.size() << " project(s)\n";
            for(const auto& s : savingErrors)
                std::cerr << "\t" << s << '\n';
            return false;
        }
        else{
            std::cout << "Saved all projects.\n";
            return true;
        }
    }
    // Save by project name
    else{
        // Check project exists
        ProjectPtr projectPtr { tempProjectManager->findProject(args[0]) };
        if(!projectPtr){
            std::cerr << "Could not find project \"" << args[0] << "\"\n";
            return false;
        }
        else{
            // Attempt to save
            const Project& projectBuffer { *projectPtr };
            if(!tempFileManager->writeProject(tempSettings->getProjectDirectory(), projectBuffer)){
                std::cerr << "Could not save project \"" << args[0] << "\"\n";
                return false;
            }
            else{
                std::cout << "Saved project \"" << projectBuffer.getName() << "\"\n";
                return true;
            }
        }
    }
}
// MARK: Print
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
    this->description = "<arg>\nversion \tPrints version information\n"
                                "total-time \tPrint total time of selected project\n"
                                "settings \tPrint settings\n"
                                "is-running \tShow if a timer is running\n"
                                "time \t\tShows current clock time\n"
                                "runtime \tShows duration of current timer\n";
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
    else if(args[0] == "total-time"){
        if(tempProjectManager->getSelectedProject()){
            std::cout << "\tTotal time for this project is: " << tempProjectManager->getSelectedProject()->printTotalTime().substr(0,8) <<"  [HH:MM:SS]\n";
            return true;
        }
        else{
            std::cerr << "\tThere is no project selected.\n";
            return false;
        }   
    }
    else if(args[0] == "settings"){
        std::cout << "Project Directory:          \"" << tempSettings->getProjectDirectory() << "\"\n";
        std::cout << "Verbose Mode:               "   << std::boolalpha   << tempSettings->getVerbose() << "\n";
        std::cout << "Auto Save:                  ";
        
        tempSettings->getIsAutoSaveOn() ?  std::cout << "on\n" : std::cout << "off\n";

        std::cout << "Entry Name Text Box Width:  "   << (int)tempSettings->getEntryNameWidth() << "\n";
        std::cout << "Entries Shown Per Page:     "   << (int)tempSettings->getEntriesPerPage() << "\n";
        return true;
    }
    else if(args[0] == "is-running"){
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
    else if(args[0] == "time"){
        Timestamp now;
        now.stamp();
        std::cout << "\t" << now.printTime().substr(0, 8) << " " << now.printDate() << std::endl;
        return true;
    }
    else if(args[0] == "runtime"){
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
    else if(args[0] == "version"){
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
                        "verbose <bool>             \tTrue or false to set the verbosity of outputs\n"
                        "auto-save <bool>           \tTrue or false to turn on auto saving. Auto saving occurs when on the home screen and there is a timer running\n"
                        "project-directory <string> \tSets the project directory\n"
                        "entry-name-width <int>     \tSets the width of the name of entries\n"
                        "entries-per-page <int>     \tThis number of entries will be displayed each time\n";
}
bool Set::execute(const std::vector<std::string>& args){
    std::shared_ptr<Settings> tempSettings { settings.lock() };

    if(!tempSettings)
        return false;


    if(args.size() > 2){
        std::cerr << "\tThis command requires two arguments.\n";
        return false;
    }
    else if(args[0] == "verbose"){
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
    else if(args[0] == "auto-save"){
        if (args[1] == "true" || args[1] == "TRUE"){
            tempSettings->setAutoSave(true);
            return true;
        }
        else if (args[1] == "false" || args[1] == "FALSE"){
            tempSettings->setAutoSave(false);
            return true;
        }
        else{
            std::cout << "\tInvalid argument.\n";
            return false;
        }
    }
    else if(args[0] == "project-directory"){
        if(tempSettings->setProjectDirectory(args[1])){
            return true;
        }
        else{
            std::cerr << "\tCould not set project save directory to \"" << args[1] << "\"\n";
            return false;
        }
    }
    else if(args[0] == "entry-name-width"){
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
    else if(args[0] == "entries-per-page"){
        try{
            int perPage {std::stoi(args[1])};
            tempSettings->setEntriesPerPage(perPage);
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
Timeline::Timeline(std::string command, std::weak_ptr<ProjectManager> setProjectManager, std::weak_ptr<Settings> setSettings) : ProjectCommand { command, setProjectManager }, settings { setSettings} {
	this->description = "Displays a timeline of tracked timers of a specified timeframe\n";	
}
bool Timeline::execute(const std::vector<std::string>& args){
	using EntryID 		= Timeline::EntryID;
	using EntryPoints 	= Timeline::EntryPoints;
	using TimelineDay	= Timeline::TimelineDay;

	using namespace CommandHelper;


	std::shared_ptr<Settings> tempSettings { settings.lock() };
	std::shared_ptr<ProjectManager> tempManager { projectManager.lock() };

	if(!tempSettings || !tempManager)
		return false;

	char option { 'w' };
	std::chrono::time_point<std::chrono::system_clock> backstop;
	std::chrono::time_point<std::chrono::system_clock> frontstop { std::chrono::system_clock::now() };
	std::array<TimelineDay, MAX_TIMELINE_DAYS> timelineDays;
	int timelineDaysIndex { 0 };

	// Put correct mode and set the proper backstop
	if(!args.empty())
		option = args[0][0];

	switch(option){
		case 'w':
			backstop = getBeginningOfWeek(frontstop);
			break;
		case '7':
			backstop = getNumDaysAgo(7, frontstop);
			break;
		default:
			if(isdigit(option))
				backstop = getNumDaysAgo((int)option - 48, frontstop);
			else
				backstop = getBeginningOfWeek(frontstop);
			break;
	}

	// Get all entries from projects before the backstop
	std::vector<std::shared_ptr<const Project>> projects { tempManager->getAllProjects() };
	std::map<std::chrono::system_clock::time_point, EntryPair> sortedEntries;

	for(const auto& projPtr : projects){
		const Project& proj { *projPtr};
		
		 std::vector<EntryPtr> entries {};
		 proj.getEntries(entries);

		for(const auto& entryPtr : entries){
			if(entryPtr->getRawStartTime().get_sys_time() > backstop)
				sortedEntries.try_emplace(std::chrono::time_point{entryPtr->getRawStartTime().get_sys_time()}, EntryPair{entryPtr, std::string{proj.getName()}} );
		}
	}
	
	// Early exit if no entries in the timeframe
	if(sortedEntries.empty()){
		std::cout << std::format ("There were no Entries between {:%F}", backstop ) << " and " << std::format("{:%F}", frontstop) << "\n";
		return true;
	}

	// Loop variables
	int daysToDisplay { (std::chrono::floor<std::chrono::days>(frontstop) - std::chrono::floor<std::chrono::days>(backstop)).count() }; // Set loop limit
	char entryID { 'A' };
	char overlapID { 'a' }; // If entries overlap, assign ID starting at A
	std::map<std::chrono::system_clock::time_point, EntryPair>::iterator itr { sortedEntries.begin() };
	
	// Breakdown entries by day
	for(int i { 1 }; i <= daysToDisplay; ++i){
		std::chrono::zoned_time zonedDay {std::chrono::current_zone(), backstop + std::chrono::days(i)}; 

		int entryArrayIndex { 0 };
		std::array<EntryID, 10> entriesForDay { };

		while(itr != sortedEntries.end()) {
			if(entryArrayIndex >= 10) // Do not add more than 10 entries per day
				break;

			const auto zonedDayIndexFloor { std::chrono::floor<std::chrono::days>(zonedDay.get_local_time()) };
			const auto zonedDayItrFloor { std::chrono::floor<std::chrono::days>(itr->second.entry->getRawStartTime().get_local_time()) };

			// If days are the same for the index and the row, add to entriesForDay array
			// If index floor is less than the itr, increment itr and repeat
			if(zonedDayIndexFloor > zonedDayItrFloor)
				++itr;
			else if(zonedDayIndexFloor == zonedDayItrFloor){
				entriesForDay[entryArrayIndex] = EntryID{entryID++, itr->second.entry};
				++entryArrayIndex;
				++itr;
			}
			else	
				break;
		}

		if(entryArrayIndex >= 0 && entryArrayIndex < 10){
			// Make start and end points so renderTimeline knows where to put proper characters (with overlapping points condensed)
			timelineDays.at(timelineDaysIndex) = {zonedDay, makeTimelineEntryPoints(zonedDay, entriesForDay, overlapID)};
			++timelineDaysIndex;
		}	
	}

	std::ostringstream timeline { };
	timeline << renderTimeline(timelineDays, daysToDisplay).str(); 

	std::cout << timeline.str();
	
	return true;
}


// MARK: Command Helper
termios CommandHelper::enableRawMode(){
    termios originalAttributes;
    termios modifiedAttributes;

    if (!isatty (STDIN_FILENO)) 
        throw std::domain_error("Not a terminal, Cannot put into non-canonical mode");
    
    // Save the terminal attributes
    tcgetattr (STDIN_FILENO, &originalAttributes);


    // Set the modified terminal modes. 
    tcgetattr (STDIN_FILENO, &modifiedAttributes);
    modifiedAttributes.c_lflag &= ~(ICANON|ECHO);   // Clear ICANON and ECHO
    modifiedAttributes.c_cc[VMIN] = 1;              // Sets min number of bytes that must be available for read() to return
    modifiedAttributes.c_cc[VTIME] = 0;             // The TIME slot is only meaningful in noncanonical input mode; it specifies how long to wait for input before returning, in units of 0.1 seconds. 
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &modifiedAttributes);

    return originalAttributes;
}
void CommandHelper::disableRawMode(const termios& attributes){
    tcsetattr (STDIN_FILENO, TCSANOW, &attributes);
}
void CommandHelper::clearRelativeTerminalSection(uint64_t moveUp, uint64_t linesToClear){

    std::ostringstream moveCursorUp;
    std::ostringstream moveCursorDownOneLine { "\033[1B"} ;
    std::ostringstream clearLine { "\033[K" };
   
    moveCursorUp << "\033[" << moveUp << "A";

    std::cout << '\r' << moveCursorUp.str();

    for(uint64_t i { 0 }; i < linesToClear; ++i){
        std::cout << clearLine.str();
        std::cout << moveCursorDownOneLine.str();
    }

    return;
}
std::chrono::time_point<std::chrono::system_clock> CommandHelper::getBeginningOfWeek(std::chrono::time_point<std::chrono::system_clock> time){
	std::chrono::weekday currentDay { std::chrono::floor<std::chrono::days>(time) } ;

	return time - std::chrono::days(static_cast<int>(currentDay.c_encoding()));
}
std::chrono::time_point<std::chrono::system_clock> CommandHelper::getNumDaysAgo(int days, std::chrono::time_point<std::chrono::system_clock> time){
	return time - std::chrono::days(days);
}
std::array<Timeline::EntryPoints, 10> CommandHelper::makeTimelineEntryPoints (timepoint day, const std::array<Timeline::EntryID, 10>& entryIDs, char& overlapID){
	struct EntryRender{
		Timeline::EntryID entryPair {};
		int start	{ 0 };
		int end 	{ 0 };
	};

	auto zonedDayFloor { std::chrono::floor<std::chrono::days>(std::chrono::zoned_time {std::chrono::current_zone(), day}.get_local_time()) };

	std::array<EntryRender, 10> entryRenders;
	int pointsIndex { 0 };

	// Determine start and end points for each timer
	for(int entryIndex { 0 }; entryIndex < entryIDs.max_size(); ++entryIndex){
		const auto& [ID, ownedEntry] {entryIDs.at(entryIndex)};
		const auto& entry { ownedEntry.entry };

		if(entry){
			int startMulOf15 { (std::chrono::duration_cast<std::chrono::minutes>(entry->getRawStartTime().get_local_time() - zonedDayFloor).count()) / 15 };
			int endMulOf15 	{ (std::chrono::duration_cast<std::chrono::minutes>(entry->getRawEndTime().get_local_time() - zonedDayFloor).count()) / 15 };

			entryRenders[pointsIndex] = EntryRender{Timeline::EntryID{ID, entry}, startMulOf15, endMulOf15};
			++pointsIndex;
		}
	}
	


	std::array<Timeline::EntryPoints, 10> finalRenderArray { };
	int finalRenderIndex { 0 };

 	for( pointsIndex = 0 ; pointsIndex < entryRenders.max_size(); ++pointsIndex){
		const EntryRender& renderStartEntry { entryRenders.at(pointsIndex) };

		if(renderStartEntry.start == 0 && renderStartEntry.end == 0)
			continue;

		Timeline::EntryPoints finalRenderBuffer { };
		int finalRenderBufferIndex { 0 };

		// Copy raw entry into final render array as the start point
		finalRenderBuffer.entryIDs.at(finalRenderBufferIndex) = renderStartEntry.entryPair;
		finalRenderBuffer.start = renderStartEntry.start;
		finalRenderBuffer.ID = renderStartEntry.entryPair.ID;		// Set initial ID for the entry
		

		// See if next point's start overlaps with current's end, add if they do
		int renderBufferEnd { renderStartEntry.end };
		for( ; pointsIndex < entryRenders.max_size()-1 && finalRenderBufferIndex < finalRenderBuffer.entryIDs.max_size()-1; ++pointsIndex){
			const EntryRender& compareEntry { entryRenders.at(pointsIndex + 1)};

			// Don't bother if the entry is nothing
			if(compareEntry.start == 0 && compareEntry.end == 0)
				continue;

			// If they overlap, add to the finalRenderBuffer's array.
			// Continue until end < start of next entry
			if(renderBufferEnd >= compareEntry.start){
				finalRenderBuffer.ID = overlapID++;							// Increment and assign overlap ID
				finalRenderBuffer.entryIDs.at(++finalRenderBufferIndex) = compareEntry.entryPair;	// Add raw entry to list of entries in the overlap
				++finalRenderBufferIndex;
				renderBufferEnd = compareEntry.end;	// Set endpoint
			}
			else
				break;

		}

		finalRenderBuffer.end = renderBufferEnd;
		
		// Add to render array
		finalRenderArray.at(finalRenderIndex) = finalRenderBuffer;
		++finalRenderIndex;
	}

	return finalRenderArray;
}
std::ostringstream CommandHelper::renderTimeline(const std::array<Timeline::TimelineDay, MAX_TIMELINE_DAYS>& timelineDays, int daysToDisplay){
	std::ostringstream os;
	int timelineWidth 	{ 96 };	// Width of entire table
	int labelWidth		{ 5 };	// Width of labels for each day

	if(daysToDisplay <= 0){
		os << "Invalid number of days to display [" << daysToDisplay << "]\n";
		return os;
	}

	if(daysToDisplay > timelineDays.max_size())
		daysToDisplay = timelineDays.max_size();

	for(int i { 0 }; i < daysToDisplay; ++i){
		auto timelineDay 	{ timelineDays.at(i) };
		auto day 		{ std::chrono::floor<std::chrono::days>(std::chrono::zoned_time {std::chrono::current_zone(), timelineDay.day}.get_local_time()) };

		os << std::format("{:%a}", day) << " |";

		int entryPointIndex { 0 };	

		// Early continue if there are no entries for that day
		if(timelineDay.entryPoints.at(entryPointIndex).start == 0 && timelineDay.entryPoints.at(entryPointIndex).end == 0){
			os << '\n';
			continue;
		}

		// Plot initial point		
		int charPosition { 0 };
		for( ; charPosition < timelineWidth; ++charPosition){
			if(entryPointIndex >= timelineDays.max_size())
				break;

			Timeline::EntryPoints& entryPoint { timelineDay.entryPoints.at(entryPointIndex) };	

			if(charPosition == entryPoint.start){
				std::string render { CommandHelper::makeTimelineEntryRender(entryPoint) };
				os << render << ' ';
					
				charPosition += render.size();
				++entryPointIndex;	
				continue;
			}

			os << ' ';
		}
		os << "\n";
	}	

	// Printing of bottom line
	for(int i { 0 }; i < timelineWidth + labelWidth; ++i){
		os << '_';
	}
	os << "\n     ";

	// Printing of the times
	for(int i { 0 }; i < timelineWidth; ++i){
		if(i % 4 == 0)
			os << '|';
		else
			os << ' ';
	}
	os << "\n   12am  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23\n\n";

	// Prints bottom information
	for(int i { 0 }; i < daysToDisplay; ++i){
		for(const auto& entryPoint : timelineDays.at(i).entryPoints){
		
			if(entryPoint.start == 0 && entryPoint.end == 0)
				continue;

			os << "\n\n";
			os << "Final Render:\n";
			os << "\tStart: " << entryPoint.start << "\n";
			os << "\tEnd: " << entryPoint.end << "\n";
			os << "\tEntries: \n";
			for(const auto& [ID, entryPair] : entryPoint.entryIDs){
				if(entryPair.entry)
					os << "\tID: " << ID << "| Entry: " << entryPair.entry->getRawStartTime().get_local_time() << "\n";
			}
		}	
	}
	
	return os;	
}
std::string CommandHelper::makeTimelineEntryRender	(const Timeline::EntryPoints& entryPoint){
	if(entryPoint.start == 0 && entryPoint.end == 0)
		return std::string{"Invalid EntryPoint"};

	int span { entryPoint.end - entryPoint.start };
	std::string ID { entryPoint.ID };

	// Format the entry to output
	// subtract two for the [ ] spacing
	// Span == 0 means start and end are the same, so that is one space
	
	switch(span){
		case 0:
			return ID;
			break;
		case 1:
			return std::string{ID + ']'};  
			break;
		case 2:
			return std::string{'[' + ID + ']' };
			break;
		default:
			return std::format("[{:=^{}}]", ID, span-1);  
			break;
	}
}























