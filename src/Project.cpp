#include "Project.h"

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

void Project::setName(const std::string& s){
	name = s;
}
std::string_view Project::getName() const{
	return name;
}

bool Project::setLastUpdated(Timestamp& timestamp) {
	if (timestamp.IsStamped()) {
		lastUpdated = timestamp;
		return true;
	}
	else
		return false;
}
Timestamp Project::getLastUpdated() const {
	return lastUpdated;
}
bool Project::isTimerRunning() const{
	if(runningEntry){
		return runningEntry->IsRunning();
	}
	else
		return false;
}


bool Project::addEntry(EntryPtr entry) {

	// Only add if finished
	if(entry->IsFinished()){
		entries.push_back(entry);

		// Updated lasUpdated time
		Timestamp now {};
		now.stamp();
		setLastUpdated(now);

		return true;
	}
	else
		return false;
	/* If the timer is running, add it to the list of timers
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
	// Linear search. erase if IDs are the same
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

	if (!runningEntry) {
		ProjectEntry newTimer(name);
		newTimer.start();
		runningEntry = std::make_shared<ProjectEntry>(std::move(newTimer));
		
		return true;
	}
	else
		return false;

	return false;
}
bool Project::endTimer() {

	// If there is no timer tracked, return false
	if (!runningEntry) {
		return false;
	}
	else {
		// If a timer has been found, and it was successfully stopped, add to list of entries and reset tracker
		if (runningEntry->end()) {
			addEntry(runningEntry);
			runningEntry.reset();
			return true;
		}
		else {
			// If the timer could not be stopped, return false;
			return false;
		}
	}

	return false;
}
std::shared_ptr<Timestamp> Project::getRunningTimerStartTime() const{
	if(isTimerRunning()){
		Timestamp startTime {runningEntry->getRawStartTime()};
		return std::make_shared<Timestamp>(startTime);
	}
	else{
		return nullptr;
	}
}

std::ostringstream Project::printAllEntries() const {

	std::ostringstream os;

	if (entries.empty()) {
		os << "There are no entries for this project.\n";
	}
	else {
		os << std::format("{:<20}", "Date");
		os << std::format("{:<20}", "Name");
		os << std::format("{:<20}", "Start Time");
		os << std::format("{:<20}", "End Time");
		os << std::format("{:<20}", "Duration");

		os << "\n" << std::format("{:-<100}", '-') << "\n";
		for (const auto& t : entries) {
			os << std::format("{:<20}", t->printDate());

			// Shorten lengthy names when displaying
			std::string fullName {t->getName()};
			if(fullName.length() > 19){
				std::string shortName {fullName.substr(0,16)};
				shortName += "...";
				os << std::format("{:<20}", shortName);
			}
			else
				os << std::format("{:<20}", fullName);
			
			os << std::format("{:<20}", t->printStartTime(), 2);
			os << std::format("{:<20}", t->printEndTime(), 2);
			os << std::format("{:<20}", t->printDuration(), 2);
			os << std::endl;
		}
		
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
ProjectPtr ProjectManager::getProject(){
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
	return nullptr;
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
	return false;
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
	if(projects.contains(name)){
		projects.erase(projects.find(name));
		return true;
	}
	else
		return false;
}