#include "Project.h"
#include "FileIO.h"
#include "Settings.h"

#include <Config.h>

#include <sstream>
#include <format>
#include <unistd.h>
#include <termios.h>
#include <map>
#include <chrono>

namespace TimeTracker{
    class Command{
    public:
        Command(std::string text);
        virtual ~Command();

        virtual bool execute(const std::vector<std::string>& args) = 0;
        std::string getCommand()        const;
        std::string getDescription()    const;
    private:
        std::string command { };
    protected:
        std::string description { };
    };

    class ProjectCommand : public Command{
    public:
        ProjectCommand( std::string command, std::weak_ptr<ProjectManager> setProjectManager);
        virtual ~ProjectCommand();
    protected:
        std::weak_ptr<ProjectManager>   projectManager;
    };


    // MARK: Project Commands
    class SelectProject : public ProjectCommand{
    public:
        SelectProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager);

        bool execute(const std::vector<std::string>& args) override;
    };
    class DeselectProject : public ProjectCommand{
    public:
        DeselectProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager);

        bool execute(const std::vector<std::string>& args) override;
    };
    class List : public ProjectCommand{
    public:
        List(   std::string                     command, 
                std::weak_ptr<ProjectManager>   setProjectManager, 
                std::weak_ptr<Settings>         setSettings
            );

        bool execute(const std::vector<std::string>& args) override;
    private:
        std::weak_ptr<Settings> settings;
    };
    class CreateProject : public ProjectCommand{
    public:
        CreateProject(std::string command, std::weak_ptr<ProjectManager> setProjectManager);

        bool execute(const std::vector<std::string>& args) override;
    };
    class DeleteProject : public ProjectCommand{
    public:
        DeleteProject(  std::string                     command, 
                        std::weak_ptr<ProjectManager>   setProjectManager, 
                        std::weak_ptr<FileIOManager>    setFileManager, 
                        std::weak_ptr<Settings>         setSettings
                    );

        bool execute(const std::vector<std::string>& args) override;
    private:
        std::weak_ptr<FileIOManager>    fileManager;
        std::weak_ptr<Settings>         settings;
    };
    class Reload : public ProjectCommand{
    public:
        Reload( std::string command, 
                std::weak_ptr<ProjectManager>   setProjectManager,
                std::weak_ptr<FileIOManager>    setFileManager,
                std::weak_ptr<Settings>         setSettings
            );
        ~Reload();

        bool execute(const std::vector<std::string>& args) override;
    private:
        std::weak_ptr<FileIOManager>    fileManager;
        std::weak_ptr<Settings>         settings;

    };
    class StartTimer : public ProjectCommand{
    public:
        StartTimer(std::string command, std::weak_ptr<ProjectManager> setProjectManager);

        bool execute(const std::vector<std::string>& args) override;
    };
    class EndTimer : public ProjectCommand{
    public:
        EndTimer(std::string command, std::weak_ptr<ProjectManager> setProjectManager);

        bool execute(const std::vector<std::string>& args) override;
    };


    // MARK: Regular Commands
    class Save : public Command {
    public:
        Save(   std::string                     command, 
                std::weak_ptr<ProjectManager>   setProjManager, 
                std::weak_ptr<FileIOManager>    setFileManager, 
                std::weak_ptr<Settings>         setSettings
            );
        
        bool execute(const std::vector<std::string>& args) override;
    private:
        std::weak_ptr<ProjectManager>   projectManager;
        std::weak_ptr<FileIOManager>    fileManager;
        std::weak_ptr<Settings>         settings;
    };

    class Print : public Command {
    public:
        Print(  std::string                     command, 
                std::weak_ptr<ProjectManager>   setProjManager, 
                std::weak_ptr<FileIOManager>    setFileManager, 
                std::weak_ptr<Settings>         setSettings
            );
        
        bool execute(const std::vector<std::string>& args) override;
    private:
        std::weak_ptr<ProjectManager>   projectManager;
        std::weak_ptr<FileIOManager>    fileManager;
        std::weak_ptr<Settings>         settings;
    };
    class Set : public Command {
    public:
        Set(std::string command, std::weak_ptr<Settings> setSettings);

        bool execute(const std::vector<std::string>& args);
    private:
        std::weak_ptr<Settings> settings;
    };

    class ClearScreen : public Command{
    public:
        ClearScreen(std::string command);

       bool execute(const std::vector<std::string>& args) override;
    };

	class Timeline : public ProjectCommand {
	public:	
		Timeline(std::string command, 
			std::weak_ptr<ProjectManager>   setProjectManager,
			std::weak_ptr<Settings>         setSettings
		);
		
		bool execute(const std::vector<std::string>& args) override;
		
		struct EntryID {
			int ID { 0 };
			EntryPtr entry;
		};
		struct EntryPoints {
			std::array<EntryID, 10> entries;
			int start { 0 };
			int end { 0 };
		};
		struct TimelineDay{
			timepoint day { };
			std::array<EntryPoints, 10> entries;
		};
	private:
		std::weak_ptr<Settings> settings;
	};
	

    namespace CommandHelper{
	constexpr int MAX_TIMELINE_DAYS { 31 };
        termios enableRawMode();                            // Returns the unmodified terminal attributes
        void disableRawMode(const termios& attributes);     // Applies the parameter attributes to terminal
        void clearRelativeTerminalSection(uint64_t moveUp, uint64_t linesToClear);


	std::chrono::time_point<std::chrono::system_clock> getBeginningOfWeek(std::chrono::time_point<std::chrono::system_clock> time);
	std::chrono::time_point<std::chrono::system_clock> getNumDaysAgo(int days, std::chrono::time_point<std::chrono::system_clock> time);
	
	std::array<Timeline::EntryPoints, 10> makeTimelineEntryPoints (timepoint day, const std::array<Timeline::EntryID, 10>& entries);
	std::ostringstream renderTimeline(const std::array<Timeline::TimelineDay, MAX_TIMELINE_DAYS>& entries, int daysToDisplay);
    }
}
