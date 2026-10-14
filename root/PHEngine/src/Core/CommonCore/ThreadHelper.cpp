#include "ThreadHelper.h"

ThreadHelper::ThreadHelper()
    : mKnownThreads()
    , mHasher()
{
}

ThreadHelper* ThreadHelper::GetInstance()
{
    static ThreadHelper* helper = new ThreadHelper();
    return helper;
}

void ThreadHelper::RegisterThread(const std::string& threadName)
{
    const size_t thisThreadId = mHasher(std::this_thread::get_id());
    assert(!mKnownThreads.count(thisThreadId));
    mKnownThreads.emplace(thisThreadId, threadName);
}

void ThreadHelper::UnregisterThread(const std::string& threadName)
{
    auto removeIt
        = std::find_if(mKnownThreads.begin(), mKnownThreads.end(), [&](const auto& pair) { return pair.second == threadName; });
    assert(removeIt != mKnownThreads.end());
    mKnownThreads.erase(removeIt->first);
}

std::string ThreadHelper::GetCurrentThreadNameFromRegisteredThreads() const
{
    const size_t thisThreadId = mHasher(std::this_thread::get_id());
    if (mKnownThreads.count(thisThreadId)) {
        return mKnownThreads.at(thisThreadId);
    }
    return "Unknown thread.";
}

bool ThreadHelper::IsCurrentThreadEqualToProvidedByName(const std::string& providedName)
{
    const size_t thisThreadId = mHasher(std::this_thread::get_id());
    assert(mKnownThreads.count(thisThreadId));
    return mKnownThreads.at(thisThreadId) == providedName;
}