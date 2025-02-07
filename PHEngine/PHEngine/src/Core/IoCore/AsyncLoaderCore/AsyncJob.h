#pragma once

#include <functional>
#include <future>

template<typename ReturnType, typename... InputArgs>
struct AsyncJob {
private:
    std::function<ReturnType(InputArgs...)> mJobImplementation;

public:
    AsyncJob(std::function<ReturnType(InputArgs...)>&& jobImplementation)
        : mJobImplementation(std::move(jobImplementation))
    {
    }

    std::future<ReturnType> StartAsync(InputArgs&&... args)
    {
        auto result = std::async(std::launch::async, mJobImplementation, std::forward<InputArgs>(args)...);
        return result;
    }

    std::future<ReturnType> StartDeferred(InputArgs&&... args)
    {
        auto result = std::async(std::launch::deferred, mJobImplementation, std::forward<InputArgs>(args)...);
        return result;
    }
};