#pragma once

#include "Project.h"
#include "Settings.h"
#include "json.h"

#include <fstream>
#include <sstream>

#include <iostream>

namespace TimeTracker{
    class ProjectFormatter {
    public:
        ProjectFormatter();
        virtual ~ProjectFormatter();

        virtual bool    write   (const std::string& path, const Project& project)  const = 0;
        virtual Project read    (const std::string& path)                          const = 0;
    };


    class JsonFormat final : public ProjectFormatter {
    public:
        JsonFormat();
        ~JsonFormat();

        bool    write   (const std::string& path, const Project& project)  const override;
        Project read    (const std::string& path)                          const override;
    };

    class FileIOManager{
    public:
        FileIOManager(std::shared_ptr<ProjectFormatter> setProjFormat);
        ~FileIOManager();

        ProjectPtr              readProjectFile (const std::string& path)       const;
        std::vector<ProjectPtr> readDirectory   (const std::string& directory)  const;

        bool writeProject   (const std::string& directory, const Project& project) const;
        bool deleteProject  (const std::string& path)                              const;

    private:
        std::shared_ptr<ProjectFormatter>   projectFormat{};
    };


    namespace FileIO{
        std::chrono::system_clock::time_point stringToTimepoint(const std::string& time);
    }
}
