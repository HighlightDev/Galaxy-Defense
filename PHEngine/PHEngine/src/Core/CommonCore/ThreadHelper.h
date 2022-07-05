#pragma once

#include <unordered_map>
#include <thread>

#include "Assertion.h"

class ThreadHelper
{
private:
    std::unordered_map<size_t, std::string> mKnownThreads;
    std::hash<std::thread::id> mHasher;

public:
    ThreadHelper()
        : mKnownThreads(),
          mHasher()
    {
    }

    static ThreadHelper *GetInstance()
    {
        static ThreadHelper helper;
        return &helper;
    }

    void RegisterThread(const std::string &threadName)
    {
        const size_t thisThreadId = mHasher(std::this_thread::get_id());
        assert(!mKnownThreads.count(thisThreadId));
        mKnownThreads.emplace(thisThreadId, threadName);
    }

    std::string GetThreadName() const
    {
        const size_t thisThreadId = mHasher(std::this_thread::get_id());
        assert(mKnownThreads.count(thisThreadId));
        return mKnownThreads.at(thisThreadId);
    }
};