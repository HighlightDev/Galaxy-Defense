#include "SaveSystem.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Game/Events/LevelFilesListChangedEvent.h"

#include <algorithm>
#include <fstream>

namespace {
std::filesystem::path GetLevelsPath()
{
    return EngineUtility::GetDocumentsDir() / "My Games" / "GalaxyDefense" / "Levels";
}
} // namespace

namespace Game {

SaveSystem::SaveSystem()
{
    Initialize();
}

void SaveSystem::Initialize()
{
    ScanLevelsDirectory();
    const auto levelsDirPath = EngineUtility::GetDocumentsDir() / "My Games" / "GalaxyDefense" / "Levels";
    mLevelsDirWatcher = std::make_unique<IO::FileWatcher>(
        levelsDirPath,
        std::chrono::milliseconds(1000),
        std::bind(&SaveSystem::OnLevelsDirectoryChanged, this, std::placeholders::_1, std::placeholders::_2));
}

std::vector<std::string> SaveSystem::GetLevelFilesNames() const
{
    std::vector<std::string> levelFileNames;
    {
        std::lock_guard<std::mutex> lock(mLevelFilesLock);
        levelFileNames.reserve(mLevelFiles.size());
        std::transform(mLevelFiles.begin(), mLevelFiles.end(), std::back_inserter(levelFileNames), [](const auto& pair) {
            return pair.first;
        });
    }
    return levelFileNames;
}

std::map<std::string, std::filesystem::path> SaveSystem::GetLevelPaths() const
{
    std::map<std::string, std::filesystem::path> ret;
    std::lock_guard<std::mutex> lock(mLevelFilesLock);
    ret = mLevelFiles;
    return ret;
}

void SaveSystem::SaveLevel(const std::string& levelName, const std::string& serializedLvl)
{
    LogInfo("SaveSystem::SaveLevel: ", levelName);
    std::filesystem::path pathToLvl = GetLevelsPath() / levelName;
    if (std::filesystem::exists(pathToLvl)) {
        LogInfo("SaveSystem::SaveLevel: Warning! File with the same name exists: ", levelName);
        return;
    }

    std::ofstream os;
    os.open(pathToLvl, std::fstream::out);
    os << serializedLvl;
    os.close();
}

void SaveSystem::OnLevelsDirectoryChanged(const std::string& path, IO::FileStatus fileStatus)
{
    EngineCore::LogInfo("SaveSystem::OnLevelsDirectoryChanged: path: ", path, " fileStatus: ", static_cast<int32_t>(fileStatus));
    if (IO::FileStatus::ERASED == fileStatus || IO::FileStatus::CREATED == fileStatus) {
        ScanLevelsDirectory();
    }
}

void SaveSystem::ScanLevelsDirectory()
{
    EngineCore::LogInfo("SaveSystem::ScanLevelsDirectory");
    {
        std::lock_guard<std::mutex> lock(mLevelFilesLock);
        mLevelFiles.clear();
        try {
            const auto levelsDirPath = GetLevelsPath();
            std::error_code errorCode;
            std::filesystem::create_directories(levelsDirPath, errorCode);
            if (errorCode) {
                EngineCore::LogInfo("SaveSystem::ScanLevelsDirectory: Failed to create levels directory: ", errorCode.message());
                return;
            }

            for (const auto& file : std::filesystem::recursive_directory_iterator(levelsDirPath)) {
                if (file.is_regular_file()) {
                    const auto& filePath = file.path();
                    const auto& fileName = EngineUtility::PathToUtf8(filePath.filename());
                    mLevelFiles[fileName] = EngineUtility::PathToUtf8(filePath);
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            EngineCore::LogInfo("SaveSystem::ScanLevelsDirectory: Failed to scan levels directory: ", e.what());
        }
    }

    Event::LuaLevelFilesListChangedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::POST_EXECUTION, GetLevelFilesNames());
}
} // namespace Game
