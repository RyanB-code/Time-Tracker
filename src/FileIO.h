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

    virtual bool write(std::string path, const Project& project) = 0;
    virtual Project read (std::string path) = 0;
};


class JsonFormat final : public ProjectFormatter {
public:
    JsonFormat();
    ~JsonFormat();

    bool write (std::string path, const Project& project) override;
    Project read(std::string path) override;
};


namespace FileIO{
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& time);
}


#endif