#pragma once
#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

namespace IO {
enum class FileStatus { CREATED, MODIFIED, ERASED };

class FileWatcher {
public:
    FileWatcher(
        std::string _path_to_watch,
        std::chrono::duration<int32_t, std::milli> _delay,
        std::function<void(std::string, FileStatus)> callback);

    ~FileWatcher();

    void start();

private:
    void initialize();

    bool contains(const std::string& key) const;

private:
    static std::atomic<int32_t> sInstanceCount;
    int32_t mInstanceId;
    std::thread mListenerThread;
    std::string mPathToWatch;
    std::chrono::duration<int32_t, std::milli> mDelay;
    std::function<void(std::string, FileStatus)> mCallback;
    std::unordered_map<std::string, std::filesystem::file_time_type> mPaths;
    std::atomic_bool mIsRunning = true;

#ifdef __linux__
    struct Impl;
    std::unique_ptr<Impl> mImpl;
#endif
};
} // namespace IO
