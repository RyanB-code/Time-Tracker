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

std::ostringstream Project::printAllEntries() const {

	std::ostringstream os;

	if (entries.empty()) {
		os << "No Entries for this project.";
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
			os << std::format("{:<20}", t->getName());
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
bool ProjectManager::selectProject(std::string name){
	try{
		selectedProject = projects.at(name);
		return true;
	}
	catch (std::out_of_range){
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



/*
void to_json(nlohmann::json& j, const Project& p) {

	j = nlohmann::json{
		{"name", p.getName()},
		{"updated", p.m_lastUpdated.print()}
	};

	int i{ 0 };
	nlohmann::json timers = nlohmann::json::array();

	for (const auto& t : p.m_timeEntries) {
		std::ostringstream s1; s1 << t->getRawStartTime();
		std::ostringstream s2; s2 << t->getRawEndTime();

		if (t->getName() == "") {
			std::ostringstream name;
			name << "Timer " << i;

			timers.push_back({ name.str(), s1.str(), s2.str() } );
		}
		else
			timers.push_back({ t->getName(), s1.str(), s2.str()});

		++i;
	}

	j["timers"] = timers;
}

void from_json(const nlohmann::json& j, Project& p) {
	std::string nameBuf, lastUpdatedBuf;
	
	j.at("name").get_to(nameBuf);
	j.at("updated").get_to(lastUpdatedBuf);

	Timestamp timestamp{ DateTimeHelper::stringToTimepoint(lastUpdatedBuf) };
	p.setLastUpdated(timestamp);

	p.setName(nameBuf);

	nlohmann::json t;
	j.at("timers").get_to(t);
	
	for (auto& elm : t.items()) {
		nlohmann::json obj = elm.value();
		std::string name, start, end;

		obj.at(0).get_to(name);
		obj.at(1).get_to(start);
		obj.at(2).get_to(end);

		TimeEntry t{ name };

		t.start(DateTimeHelper::stringToTimepoint(start));
		t.end(DateTimeHelper::stringToTimepoint(end));

		p.addTimer(t);
	}
}


bool ProjectHelper::createProjectFromFile(const std::string& file, Project& p) {
	std::ifstream inputFile{ std::filesystem::path{ file }, std::ios::in };

	try {
		nlohmann::json j{ nlohmann::json::parse(inputFile) };

		p = j.template get<Project>();

		return true;
	}
	catch (...) {
		return false;
	}

	return false;
}
*/