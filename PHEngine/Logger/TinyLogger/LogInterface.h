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

namespace TinyLogger
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT "File: " __FILE__ "; Line: " TOSTRING(__LINE__) "; Function: " TOSTRING(__FUNCTION__) ";"

   struct LogProxy
   {
      template <typename LogArg, typename... LogArgs>
      static void LogMessages(LogArg&& arg, LogArgs&&... args)
      {
         static std::hash<std::thread::id> hasher;

         std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
         typename std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
         std::time_t currentTime = std::chrono::system_clock::to_time_t(time);

         std::string timeFileWasChanged = std::asctime(std::localtime(&currentTime));
         timeFileWasChanged[timeFileWasChanged.size() - 1] = ' ';

         //std::string currentTimeStr = std::to_string() + " ";

         std::initializer_list<std::string> initList({ timeFileWasChanged, "Thread: " + std::to_string(hasher(std::this_thread::get_id())), arg, args... });

         Logger::GetInstance_()->EnqueuLogMessage(LogMessage(initList));
      }

#define LOG_INFO (AT)

      /* initialization should be called before any action with log*/

      static void InitLog() {

      }

      template <typename LoggerClient, typename... Args>
      static void InitLog(LoggerClient* loggerClient, Args&&... clients)
      {
         Logger::GetInstance_()->AddLoggerClient(loggerClient);
         InitLog(std::move(clients)...);
      }

      static void StartLogThread()
      {
         Logger::GetInstance_()->StartLogThread();
      }
   };
}