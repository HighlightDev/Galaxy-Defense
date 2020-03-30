#pragma once 

#include <thread>
#include <istream>
#include <utility>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <chrono> 
#include <string>

#include "Core.h"

namespace Log
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT "File: " __FILE__ "; Line: " TOSTRING(__LINE__) "; Function: " TOSTRING(__FUNCTION__) ";"

   template <typename LogArg, typename... LogArgs>
   void LogMessages(LogArg&& arg, LogArgs&&... args)
   {
      static std::hash<std::thread::id> hasher;

      std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
      typename std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
      std::time_t currentTime = std::chrono::system_clock::to_time_t(time);

      std::string timeFileWasChanged = std::asctime(std::localtime(&currentTime));

      //std::string currentTimeStr = std::to_string() + " ";

      std::initializer_list<std::string> initList({ timeFileWasChanged, "Thread: " + std::to_string(hasher(std::this_thread::get_id())), arg, args... });

      LogMessage message = LogMessage(initList);

      Logger::GetInstance_()->EnqueuLogMessage(std::move(message));
   }

#define LOG_INFO (AT)

   /* initialization should be called before any action with log*/
   void InitLog(const char* pathToFile);
}