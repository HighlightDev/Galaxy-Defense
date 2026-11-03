#include "FolderManager.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <filesystem>

namespace IO {
FolderManager::FolderManager()
    : mAbsFilesPathMap()
    , m_pathToExe()
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
    using EngineUtility::PathToUtf8;

    m_pathToExe = EngineUtility::GetExecutablePath();
    ext_assert(!m_pathToExe.empty(), "FolderManager::BuildSystemPathToFolders: m_pathToExe is empty");

    const std::filesystem::path rootPath = GetRootPath();

    for (auto it = std::filesystem::recursive_directory_iterator(rootPath); it != std::filesystem::recursive_directory_iterator();
         ++it) {
        const auto& dirEntry = *it;
        const std::string entryName = PathToUtf8(dirEntry.path().filename());

        if (std::filesystem::is_directory(dirEntry)) {
            if (IsObsoleteName(entryName)) {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (IsObsoleteName(entryName)) {
            continue;
        }

        const std::string& fileName = entryName;
        const std::filesystem::path& absPath = dirEntry.path();

        ext_assert(
            mAbsFilesPathMap.count(fileName) == 0,
            "FolderManager::BuildSystemPathToFolders: duplicate file name in resource tree: " + fileName
                + " (existing: " + PathToUtf8(mAbsFilesPathMap[fileName]) + ", new: " + PathToUtf8(absPath) + ")");

        mAbsFilesPathMap[fileName] = absPath;
        EngineCore::LogInfo("FolderManager::BuildSystemPathToFolders: indexed: ", PathToUtf8(absPath));
    }
}

std::filesystem::path FolderManager::GetAbsolutePathToRes(const std::string& fileName) const
{
    ext_assert(mAbsFilesPathMap.count(fileName), "FolderManager::GetAbsolutePathToRes: missing file: " + fileName);
    return mAbsFilesPathMap.at(fileName);
}

std::filesystem::path FolderManager::GetAbsPathToExeFile() const
{
    ext_assert(!m_pathToExe.empty(), "FolderManager::GetAbsPathToExeFile: forgot to invoke BuildSystemPathToFolders");
    return m_pathToExe;
}

std::filesystem::path FolderManager::GetRootPath() const
{
    return GetAbsPathToExeFile() / "res";
}
} // namespace IO
