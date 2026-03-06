#include "FileWatcher.h"

#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FolderManager.h"

#ifdef __linux__
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <unordered_map>
#endif

using namespace EngineCore;

namespace IO {

std::atomic<int32_t> FileWatcher::sInstanceCount = 0;

#ifdef __linux__
struct FileWatcher::Impl {
    int32_t mInotifyFd;
    std::vector<int32_t> mInotifyWatcherDescriptors;
    int32_t mEpollFd;
};
#endif

FileWatcher::FileWatcher(
    std::string _path_to_watch,
    std::chrono::duration<int32_t, std::milli> _delay,
    std::function<void(std::string, FileStatus)> callback)
    : mInstanceId(sInstanceCount++)
    , mPathToWatch(_path_to_watch)
    , mDelay(_delay)
    , mCallback(callback)
#ifdef __linux__
    , mImpl(std::make_unique<Impl>())
#endif
{
    LogInfo("FileWatcher::ctor");
    initialize();
}

FileWatcher::~FileWatcher()
{
    LogInfo("FileWatcher::dctor");
    mIsRunning = false;
#ifdef __linux__
    for (const auto& inotifyWatcherFd : mImpl->mInotifyWatcherDescriptors) {
        epoll_ctl(mImpl->mEpollFd, EPOLL_CTL_DEL, inotifyWatcherFd, nullptr);
        inotify_rm_watch(mImpl->mInotifyFd, inotifyWatcherFd);
    }
    if (mImpl->mInotifyFd != -1) {
        close(mImpl->mInotifyFd);
    }
    if (mImpl->mEpollFd != -1) {
        close(mImpl->mEpollFd);
    }
#endif
    ThreadHelper::GetInstance()->UnregisterThread("FileWatcherThread_" + std::to_string(mInstanceId));
    if (mListenerThread.joinable()) {
        mListenerThread.join();
    }
}

bool FileWatcher::contains(const std::string& key) const
{
    return mPaths.count(key) > 0;
}

#ifdef __linux__
void FileWatcher::initialize()
{
    mImpl->mInotifyFd = inotify_init1(IN_NONBLOCK);
    for (auto& file : std::filesystem::recursive_directory_iterator(mPathToWatch)) {
        if (file.is_directory()) {
            const int32_t watchDescriptor
                = inotify_add_watch(mImpl->mInotifyFd, file.path().string().c_str(), IN_CREATE | IN_CLOSE_WRITE | IN_DELETE);
            if (watchDescriptor == -1) {
                LogInfo("FileWatcher::initialize: Failed to add inotify watch for path: " + file.path().string());
                return;
            }

            mImpl->mInotifyWatcherDescriptors.emplace_back(watchDescriptor);
        }
    }

    if (mImpl->mInotifyWatcherDescriptors.empty()) {
        LogInfo("FileWatcher::initialize: No directories to watch in path: " + mPathToWatch);
        return;
    }

    mImpl->mEpollFd = epoll_create1(0);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = mImpl->mInotifyFd;

    if (epoll_ctl(mImpl->mEpollFd, EPOLL_CTL_ADD, mImpl->mInotifyFd, &event) == -1) {
        LogInfo(
            "FileWatcher::initialize: Failed to add inotify fd to epoll instance for fd: " + std::to_string(mImpl->mInotifyFd));
        return;
    }

    mListenerThread = std::thread(std::bind(&FileWatcher::start, this));
}

void FileWatcher::start()
{
    ThreadHelper::GetInstance()->RegisterThread("FileWatcherThread_" + std::to_string(mInstanceId));
    while (mIsRunning) {
        epoll_event events[1];
        const int32_t eventCount = epoll_wait(mImpl->mEpollFd, events, 1, 1000); // wait for 1000 ms

        if (eventCount > 0 && mImpl->mInotifyFd == events[0].data.fd) {
            char buffer[1024];
            const int32_t changedInotifyFd = events[0].data.fd;
            const ssize_t length = read(changedInotifyFd, buffer, sizeof(buffer));
            if (length > 0) {
                inotify_event* event = reinterpret_cast<inotify_event*>(buffer);
                if (event->mask & IN_CREATE) {
                    mCallback(SLASH + std::string(event->name), FileStatus::CREATED);
                } else if (event->mask & IN_CLOSE_WRITE) {
                    mCallback(SLASH + std::string(event->name), FileStatus::MODIFIED);
                } else if (event->mask & IN_DELETE) {
                    mCallback(SLASH + std::string(event->name), FileStatus::ERASED);
                }
            } else {
                LogInfo("FileWatcher::start: Failed to read inotify events");
            }
        }
    }
}
#else
void FileWatcher::initialize()
{
    for (auto& file :
         std::filesystem::recursive_directory_iterator(FolderManager::GetInstance()->GetPathToExeFile() + mPathToWatch)) {
        mPaths[file.path().string()] = std::filesystem::last_write_time(file);
    }
    mListenerThread = std::thread(std::bind(&FileWatcher::start, this));
}

void FileWatcher::start()
{
    ThreadHelper::GetInstance()->RegisterThread("FileWatcherThread_" + std::to_string(mInstanceId));
    while (mIsRunning) {
        // Wait for "mDelay" milliseconds
        std::this_thread::sleep_for(mDelay);
        auto it = mPaths.begin();
        while (it != mPaths.end()) {
            if (!std::filesystem::exists(it->first)) {
                mCallback(it->first, FileStatus::ERASED);
                it = mPaths.erase(it);
            } else {
                it++;
            }
        }

        for (auto& file :
             std::filesystem::recursive_directory_iterator(FolderManager::GetInstance()->GetPathToExeFile() + mPathToWatch)) {
            auto currentFileLastWriteTime = std::filesystem::last_write_time(file);

            if (!contains(file.path().string())) // File creation
            {
                mPaths[file.path().string()] = currentFileLastWriteTime;
                mCallback(file.path().string(), FileStatus::CREATED);
            } else // File modification
            {
                if (mPaths[file.path().string()] != currentFileLastWriteTime) {
                    mPaths[file.path().string()] = currentFileLastWriteTime;
                    mCallback(file.path().string(), FileStatus::MODIFIED);
                }
            }
        }
    }
}
#endif
} // namespace IO
