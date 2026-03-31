#pragma once

#include <cassert>
#include <string>
#include <thread>
#include <unordered_map>

class ThreadHelper {
private:
    std::unordered_map<size_t, std::string> mKnownThreads;
    std::hash<std::thread::id> mHasher;

public:
    ThreadHelper()
        : mKnownThreads()
        , mHasher()
    {
    }

    static ThreadHelper* GetInstance()
    {
        static ThreadHelper* helper = new ThreadHelper();
        return helper;
    }

    void RegisterThread(const std::string& threadName)
    {
        const size_t thisThreadId = mHasher(std::this_thread::get_id());
        assert(!mKnownThreads.count(thisThreadId));
        mKnownThreads.emplace(thisThreadId, threadName);
    }

    void UnregisterThread(const std::string& threadName)
    {
        auto removeIt = std::find_if(
            mKnownThreads.begin(), mKnownThreads.end(), [&](const auto& pair) { return pair.second == threadName; });
        assert(removeIt != mKnownThreads.end());
        mKnownThreads.erase(removeIt->first);
    }

    std::string GetCurrentThreadNameFromRegisteredThreads() const
    {
        const size_t thisThreadId = mHasher(std::this_thread::get_id());
        if (mKnownThreads.count(thisThreadId)) {
            return mKnownThreads.at(thisThreadId);
        }
        return "Unknown thread.";
    }

    bool IsCurrentThreadEqualToProvidedByName(const std::string& providedName)
    {
        const size_t thisThreadId = mHasher(std::this_thread::get_id());
        assert(mKnownThreads.count(thisThreadId));
        return mKnownThreads.at(thisThreadId) == providedName;
    }
};