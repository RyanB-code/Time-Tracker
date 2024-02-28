#ifndef FILEIO_H
#define FILEIO_H

#include "Project.h"
#include "Settings.h"
#include "json.h"

#include <fstream>
#include <sstream>

#include <iostream>


class ProjectFormatter {
public:
    ProjectFormatter();
    virtual ~ProjectFormatter();

    virtual bool write(std::string path, const Project& project) const = 0;
    virtual Project read (std::string path) const = 0;
};


class JsonFormat final : public ProjectFormatter {
public:
    JsonFormat();
    ~JsonFormat();

    bool write (std::string path, const Project& project) const override;
    Project read(std::string path) const override;
};

class SettingsFormatter {
public:
    SettingsFormatter();
    virtual ~SettingsFormatter();

    virtual bool write(std::string path, const Settings& settings) const = 0;
    virtual Settings read (std::string path) const = 0;
};

class SettingsJsonFormat final : public SettingsFormatter {
public:
    SettingsJsonFormat();
    ~SettingsJsonFormat();

    bool write (std::string path, const Settings& settings) const override;
    Settings read(std::string path) const override;
};


class FileIOManager{
public:
    FileIOManager(std::shared_ptr<ProjectFormatter> setProjFormat, std::shared_ptr<SettingsFormatter> setSettingsFormat);
    ~FileIOManager();

    ProjectPtr readProjectFile(std::string path) const;
    std::shared_ptr<Settings> readSettingsFile(std::string path="") const;
    
    std::vector<ProjectPtr> readDirectory (std::string directory="") const;

    bool writeProject(const Project& project, std::string path="") const;
    bool writeSettings(const Settings& settings, std::string path="") const;

    std::string getHomeDirectory()      const;
    std::string getProjectDirectory()   const;
    std::string getSettingsPath()       const;

    bool setHomeDirectory       (std::string path);
    bool setProjectDirectory    (std::string path);
private:
    std::string homeDirectory       {};
    std::string projectDirectory    {};
    std::string settingsPath        {};

    std::shared_ptr<ProjectFormatter>   projectFormat{};
    std::shared_ptr<SettingsFormatter>  settingsFormat{};

    bool ensureSettingsFileExists ();

};


namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time);
}


#endif