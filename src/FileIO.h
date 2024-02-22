#ifndef FILEIO_H
#define FILEIO_H

#include "Project.h"
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

class FileIOManager{
public:
    FileIOManager(std::shared_ptr<ProjectFormatter> format);
    ~FileIOManager();

    ProjectPtr readFile(std::string path) const;
    std::vector<ProjectPtr> readDirectory (std::string directory="") const;

    bool writeProject(const Project& project, std::string path="") const;

    std::string_view getDirectory() const;
    bool setDirectory(std::string path);
private:
    std::string saveDirectory {};
    std::shared_ptr<ProjectFormatter> saveFormat{};

};


namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time);
}


#endif