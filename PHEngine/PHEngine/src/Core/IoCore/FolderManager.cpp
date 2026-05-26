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

// std::filesystem::path::string() uses the system ANSI code page on Windows and throws
// std::system_error when the path contains characters outside that code page.
// This helper always produces a valid UTF-8 std::string regardless of the locale.
std::string PathToUtf8(const std::filesystem::path& p)
{
    // In C++20 u8string() returns std::u8string (char8_t), not std::string.
    // The underlying bytes are identical to UTF-8 char bytes, so the range
    // constructor is the safest zero-copy way to obtain a std::string.
    auto u8 = p.u8string();
    return {u8.begin(), u8.end()};
}
} // namespace

void FolderManager::BuildSystemPathToFolders()
{
    m_pathToExe = EngineUtility::GetExecutablePath();
    ext_assert(m_pathToExe != "", "FolderManager::BuildSystemPathToFolders: m_pathToExe is empty");

    const std::string rootPath = GetRootPath();

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

        const std::string fileName = entryName;
        const std::string absPath = PathToUtf8(dirEntry.path());

        ext_assert(
            mAbsFilesPathMap.count(fileName) == 0,
            "FolderManager::BuildSystemPathToFolders: duplicate file name in resource tree: " + fileName
                + " (existing: " + mAbsFilesPathMap[fileName] + ", new: " + absPath + ")");

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
