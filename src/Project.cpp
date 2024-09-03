#include "Project.h"

using namespace TimeTracker;

ProjectEntry::ProjectEntry(const std::string& s) : name{s} {
	setID();
}
ProjectEntry::~ProjectEntry(){

}
std::string_view ProjectEntry::getName() const{
	return name;
}
void ProjectEntry::setName(const std::string& s){
	name = s;
}
int ProjectEntry::getID() const {
	return id;
}
int ProjectEntry::setID(){
	static int idEntries{0};
	++idEntries;
	id = idEntries;
	return idEntries;
}




Project::Project(const std::string& s) : name{ s } {
	runningEntry.reset();
}
Project::~Project() {
	entries.clear();
}

void Project::setName(const std::string& setName){
	name = setName;
}
std::string_view Project::getName() const{
	return name;
}

bool Project::setLastUpdated(Timestamp& timestamp) {
	if (!timestamp.IsStamped())
		return false;
		
	lastUpdated = timestamp;
	return true;
}
Timestamp Project::getLastUpdated() const {
	return lastUpdated;
}
bool Project::isTimerRunning() const{
	if(!runningEntry)
		return false;

	return runningEntry->IsRunning();
}
uint64_t Project::getEntriesSize() const{
	return entries.size();
}

bool Project::addEntry(EntryPtr entry) {
	if(!entry->IsFinished())
		return false;

	entries.push_back(entry);

	// Updated lasUpdated time
	Timestamp now {};
	now.stamp();
	setLastUpdated(now);

	return true;

	// Meant to add to list of timers even if running
	/*
	if (entry->IsRunning()) 
	{
		if (!m_currentTimer) {
			m_timeEntries.push_back(std::make_unique<TimeEntry>(t));

			m_currentTimer = m_timeEntries.back().get();
			return true;
		}
		else
			return false;
		
	}
	else if (!t.isFinished() && !t.isRunning()) {
		return false;
	}
	else if (t.isFinished()) {
		m_timeEntries.push_back(std::make_unique<TimeEntry>(t));




		// SORTING
		//if (m_timeEntries.size() == 1 || t.getRawStartTime() > m_timeEntries[m_timeEntries.size() - 2].getRawStartTime()) {} // Does nothing if no need to sort
		//else {

		//	// Sort the m_timeEntries by start time
		//	for (int i{ 0 }; i < m_timeEntries.size() - 1; ++i) {
		//		for (int x{ i + 1 }; x < m_timeEntries.size(); ++x) {
		//			if (m_timeEntries[x].getRawStartTime() < m_timeEntries[i].getRawStartTime()) {
		//				TimeEntry t1{ m_timeEntries[x] };
		//				TimeEntry t2{ m_timeEntries[i] };

		//				m_timeEntries.em;
		//			}
		//		}
		//	}
		//}

		return true;
	}
	else
		return false;
	*/
}
bool Project::removeEntry(int id){
	// Linear search. Erase if IDs are the same
	for(std::vector<EntryPtr>::iterator it {entries.begin()}; it < entries.end(); ++it){
		if(id == it->get()->getID()){
			entries.erase(it);

			// Updated lasUpdated time
			Timestamp now {};
			now.stamp();
			setLastUpdated(now);

			return true;
		}
	}
	return false;
}
const std::vector<EntryPtr>& Project::getEntries() const{
	return entries;
}


bool Project::startTimer(std::string name) {
	if (runningEntry)	// Do not do anything if a timer is already running
		return false;
	
	ProjectEntry newTimer(name);
	newTimer.start();
	runningEntry = std::make_shared<ProjectEntry>(std::move(newTimer));
	
	return true;
}
bool Project::startTimer(timepoint startTime, std::string name){
	if (runningEntry)	// Do not do anything if a timer is already running
		return false;

	ProjectEntry newTimer(name);
	newTimer.start(startTime);
	runningEntry = std::make_shared<ProjectEntry>(std::move(newTimer));

	return true;	
}
bool Project::endTimer() {

	// If there is no timer tracked, return false
	if (!runningEntry) 
		return false;
	
	// If a timer has been found, and it was successfully stopped, add to list of entries and reset tracker
	if (runningEntry->end()) {
		addEntry(runningEntry);
		runningEntry.reset();
		return true;
	}

	// If the timer could not be stopped, return false;
	return false;
}
std::shared_ptr<Timestamp> Project::getRunningTimerStartTime() const{
	if(!isTimerRunning())
		return nullptr;
	
	Timestamp startTime {runningEntry->getRawStartTime()};
	return std::make_shared<Timestamp>(startTime);
}

std::ostringstream Project::printAllEntries(uint8_t entryNameWidth) const {
	std::ostringstream os;

	if (entries.empty()) {
		os << "There are no entries for this project.\n";
		return os;
	}

	if(entryNameWidth < 20)
		entryNameWidth = 20;

	constexpr uint8_t dateWidth {14}, timestampWidth{11};

	os << std::format("{:<{}}", "Date",		dateWidth);	
	os << std::format("{:<{}}", "Name", 	entryNameWidth); 
	os << std::format("{:<{}}", "Start", 	timestampWidth);
	os << std::format("{:<{}}", "End", 		timestampWidth);
	os << std::format("{:<{}}", "Duration", timestampWidth);

	// 44 is the number width of the text boxes to ensure line of dashes is the same
	os << "\n" << std::format("{:-<{}}", '-', (44+entryNameWidth)) << "\n";

	for (const auto& t : entries) {
		os << std::format("{:{}}", t->printDate(), dateWidth);
		std::string name {t->getName()};

		// Shorten lengthy names when displaying
		if(name.length() > ((size_t)entryNameWidth-1)){
			name = name.substr(0,(entryNameWidth-4)); // Minus 4 because emplacing ellipsis (3 chars) and then a space
			name += "...";
		}

		os << std::format("{:<{}}", name, entryNameWidth);
		os << std::format("{:<{}}", t->printStartTime().substr(0, 8), 	timestampWidth);
		os << std::format("{:<{}}", t->printEndTime().substr(0, 8), 	timestampWidth);
		os << std::format("{:<{}}", t->printDuration().substr(0, 8), 	timestampWidth);
		os << std::endl;
	}
		
	return os;
}

std::string	Project::printTotalTime() const {
	std::chrono::system_clock::duration totalTime{};

	if (entries.empty()) {
		return "00:00:00";
	}
	else if (entries.size() == 1) {
		return entries[0]->printDuration();
	}
	else {
		// Sum of all durations in entries
		for (const auto& t : entries) {
			totalTime = totalTime + t->getRawDuration();
		}
	}
	return std::format("{:%H:%M:%S}", totalTime);
}



ProjectManager::ProjectManager(){

}
ProjectManager::~ProjectManager(){

}
ProjectPtr ProjectManager::getSelectedProject() const{
	if(ProjectPtr proj = selectedProject.lock()){
		return proj;
	}
	else{
		return nullptr;
	}
}
std::vector<std::string> ProjectManager::getAllProjectNames() const{
	std::vector<std::string> names;
	for(auto i{projects.begin()}; i != projects.end(); ++i){
		names.push_back(i->first);
	}
	return names;
}
std::vector<std::shared_ptr<const Project>> ProjectManager::getAllProjects() const{
	std::vector<std::shared_ptr<const Project>> projectBuffer;
	for(auto i{projects.begin()}; i != projects.end(); ++i){
		projectBuffer.push_back(i->second);
	}
	return projectBuffer;
}

ProjectPtr ProjectManager::findProject(std::string name) const{
	try{
		return projects.at(name);
	}
	catch (std::out_of_range& e){
		return nullptr;
	}
}
bool ProjectManager::selectProject(std::string name){
	try{
		selectedProject = projects.at(name);
		return true;
	}
	catch (std::out_of_range& e){
		selectedProject.reset();
		return false;
	}
}
void ProjectManager::deselectProject() {
	selectedProject.reset();
}
bool ProjectManager::addProject(ProjectPtr project){
	if(projects.contains( std::string{project->getName()} )){
		return false;
	}
	else{
		projects.try_emplace(std::string{project->getName()}, project);
		return true;
	}
	return false;
}

bool ProjectManager::deleteProject(std::string name){
	if(!projects.contains(name))
		return false;
	
	projects.erase(projects.find(name));
	return true;
}
void ProjectManager::clearProjects(){
	projects.erase(projects.begin(), projects.end());
}