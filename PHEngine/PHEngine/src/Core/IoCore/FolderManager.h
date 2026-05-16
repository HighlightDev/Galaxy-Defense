#pragma once
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <string>
#include <unordered_map>

namespace IO {
class FolderManager {

private:
    std::unordered_map<std::string /*file name*/, std::string /*abs path to file*/> mAbsFilesPathMap;
    std::string m_pathToExe;

public:
    FolderManager();
    ~FolderManager();

    static FolderManager* GetInstance()
    {
        static FolderManager instance;
        return &instance;
    }

    void BuildSystemPathToFolders();

    std::string GetAbsolutePath(const std::string& fileName) const;
    std::string GetPathToExeFile() const;
    std::string GetRootPath() const;
};
} // namespace IO
