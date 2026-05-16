#include "FolderManager.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <filesystem>

namespace IO {
FolderManager::FolderManager()
    : mAbsFilesPathMap()
    , m_pathToExe("")
{
}

FolderManager::~FolderManager()
{
}

namespace {
bool IsObsoleteName(const std::string& name)
{
    return name == "obsolete" || name == "Obsolete";
}
} // namespace

void FolderManager::BuildSystemPathToFolders()
{
    m_pathToExe = EngineUtility::GetExecutablePath();
    ext_assert(m_pathToExe != "", "FolderManager::BuildSystemPathToFolders: m_pathToExe is empty");

    const std::string rootPath = GetRootPath();

    for (auto it = std::filesystem::recursive_directory_iterator(rootPath);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        const auto& dirEntry = *it;
        const std::string entryName = dirEntry.path().filename().string();

        if (std::filesystem::is_directory(dirEntry)) {
            if (IsObsoleteName(entryName)) {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (IsObsoleteName(entryName)) {
            continue;
        }

        const std::string fileName = entryName;
        const std::string absPath = dirEntry.path().string();

        ext_assert(
            mAbsFilesPathMap.count(fileName) == 0,
            "FolderManager::BuildSystemPathToFolders: duplicate file name in resource tree: " + fileName + " (existing: "
                + mAbsFilesPathMap[fileName] + ", new: " + absPath + ")");

        mAbsFilesPathMap[fileName] = absPath;
        EngineCore::LogInfo("FolderManager::BuildSystemPathToFolders: indexed: ", absPath);
    }
}

std::string FolderManager::GetAbsolutePath(const std::string& fileName) const
{
    ext_assert(mAbsFilesPathMap.count(fileName), "FolderManager::GetAbsolutePath: missing file: " + fileName);
    return mAbsFilesPathMap.at(fileName);
}

std::string FolderManager::GetPathToExeFile() const
{
    ext_assert(m_pathToExe != "", "FolderManager::GetPathToExeFile: forgot to invoke BuildSystemPathToFolders");
#ifdef _WIN32
    return m_pathToExe + SLASH;
#elif __linux__
    return m_pathToExe;
#endif
}

std::string FolderManager::GetRootPath() const
{
#ifdef _WIN32
    static std::string pathToRes = GetPathToExeFile() + SLASH + "res" + SLASH;
#elif __linux__
    static std::string pathToRes = GetPathToExeFile() + "res" + SLASH;
#endif
    return pathToRes;
}
} // namespace IO
