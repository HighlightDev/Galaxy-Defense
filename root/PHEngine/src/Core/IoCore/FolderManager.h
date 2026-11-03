#pragma once
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace IO {
class FolderManager {

private:
    std::unordered_map<std::string /*file name*/, std::filesystem::path /*abs path to file*/> mAbsFilesPathMap;
    std::filesystem::path m_pathToExe;

public:
    FolderManager();
    ~FolderManager();

    static FolderManager* GetInstance()
    {
        static FolderManager instance;
        return &instance;
    }

    void BuildSystemPathToFolders();

    std::filesystem::path GetAbsolutePathToRes(const std::string& fileName) const;
    std::filesystem::path GetAbsPathToExeFile() const;
    std::filesystem::path GetRootPath() const;
};
} // namespace IO
