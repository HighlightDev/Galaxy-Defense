#include "FileWatcher.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/IoCore/FolderManager.h"

using namespace EngineCore;

namespace IO
{
    FileWatcher::FileWatcher(std::string _path_to_watch, std::chrono::duration<int, std::milli> _delay,
                             std::function<void(std::string, FileStatus)> callback)
        : mPathToWatch(_path_to_watch),
          mDelay(_delay),
          mCallback(callback)
    {
        LogInfo("FileWatcher::ctor");
        initialize();
    }

    void FileWatcher::initialize()
    {
        for (auto &file : std::filesystem::recursive_directory_iterator(FolderManager::GetInstance()->GetPathToExeFile() + mPathToWatch))
        {
            mPaths[file.path().string()] = std::filesystem::last_write_time(file);
        }
        mListenerThread = std::thread(std::bind(&FileWatcher::start, this));
    }

    FileWatcher::~FileWatcher()
    {
        LogInfo("FileWatcher::dctor");
        mIsRunning = false;
        if (mListenerThread.joinable())
        {
            mListenerThread.join();
        }
    }

    bool FileWatcher::contains(const std::string &key) const
    {
        return mPaths.count(key) > 0;
    }

    void FileWatcher::start()
    {
        ThreadHelper::GetInstance()->RegisterThread("FileWatcher");
        while (mIsRunning)
        {
            // Wait for "mDelay" milliseconds
            std::this_thread::sleep_for(mDelay);
            auto it = mPaths.begin();
            while (it != mPaths.end())
            {
                if (!std::filesystem::exists(it->first))
                {
                    mCallback(it->first, FileStatus::ERASED);
                    it = mPaths.erase(it);
                }
                else
                {
                    it++;
                }
            }

            for (auto &file : std::filesystem::recursive_directory_iterator(FolderManager::GetInstance()->GetPathToExeFile() + mPathToWatch))
            {
                auto currentFileLastWriteTime = std::filesystem::last_write_time(file);

                if (!contains(file.path().string())) // File creation
                {
                    mPaths[file.path().string()] = currentFileLastWriteTime;
                    mCallback(file.path().string(), FileStatus::CREATED);
                }
                else // File modification
                {
                    if (mPaths[file.path().string()] != currentFileLastWriteTime)
                    {
                        mPaths[file.path().string()] = currentFileLastWriteTime;
                        mCallback(file.path().string(), FileStatus::MODIFIED);
                    }
                }
            }
        }
    }
}
