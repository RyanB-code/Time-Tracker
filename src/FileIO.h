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

class FileIOManager{
public:
    FileIOManager(std::shared_ptr<ProjectFormatter> setProjFormat);
    ~FileIOManager();

    ProjectPtr              readProjectFile (std::string path)       const;
    std::vector<ProjectPtr> readDirectory   (std::string directory)  const;

    bool writeProject(const Project& project, std::string directory) const;
    bool deleteProject(std::string path)                             const;

private:
    std::shared_ptr<ProjectFormatter>   projectFormat       {};

};


namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time);
}


#endif