#include "SaveSystem.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Game/Events/LevelFilesListChangedEvent.h"

#include <algorithm>

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
            const auto levelsDirPath = EngineUtility::GetDocumentsDir() / "My Games" / "GalaxyDefense" / "Levels";
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
