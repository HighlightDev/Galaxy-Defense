#pragma once

#include <future>
#include <functional>

template <typename ReturnType, typename... InputArgs>
struct AsyncJob
{
private:

   std::function<ReturnType(InputArgs...)> mJobImplementation;

public:

   AsyncJob(const std::function<ReturnType(InputArgs...)>& jobImplementation)
      : mJobImplementation(jobImplementation)
   {
   }

   std::future<ReturnType> StartAsync(const std::launch launchType, InputArgs&&... args)
   {
      auto result = std::async(std::launch::async, mJobImplementation, std::forward<InputArgs>(args)...);
      return result;
   }
};