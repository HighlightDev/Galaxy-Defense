#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <thread>
#include <unordered_map>

class ThreadHelper {
private:
    std::unordered_map<size_t, std::string> mKnownThreads;
    std::hash<std::thread::id> mHasher;

public:
    explicit ThreadHelper();

    static ThreadHelper* GetInstance();

    void RegisterThread(const std::string& threadName);

    void UnregisterThread(const std::string& threadName);

    std::string GetCurrentThreadNameFromRegisteredThreads() const;

    bool IsCurrentThreadEqualToProvidedByName(const std::string& providedName);
};