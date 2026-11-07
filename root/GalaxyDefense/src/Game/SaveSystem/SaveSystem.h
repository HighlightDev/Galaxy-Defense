#pragma once

#include "Core/IoCore/FileWatcher.h"

#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace Game {
class SaveSystem {

    std::map<std::string, std::filesystem::path> mLevelFiles;

    std::unique_ptr<IO::FileWatcher> mLevelsDirWatcher;

    mutable std::mutex mLevelFilesLock;

public:
    explicit SaveSystem();

    std::vector<std::string> GetLevelFilesNames() const;

    std::map<std::string, std::filesystem::path> GetLevelPaths() const;

    void SaveLevel(const std::string& levelName, const std::string& serializedLvl);

    static SaveSystem* GetInstance()
    {
        static SaveSystem instance;
        return &instance;
    }

private:
    void Initialize();

    void OnLevelsDirectoryChanged(const std::string& path, IO::FileStatus fileStatus);

    void ScanLevelsDirectory();
};
} // namespace Game
