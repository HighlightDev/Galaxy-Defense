#pragma once

#include <iostream>
#include <utility>
#include <stdio.h>

namespace Logger
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

   void LogMessages()
   {
   }

   template <typename LogArg, typename... LogArgs>
   void LogMessages(LogArg&& arg, LogArgs&&... args)
   {
      std::cout << arg;
      LogMessages(std::forward<LogArgs>(args)...);
   }

#define FILE_NAME_LINE_NUMBER_TO_STR (AT)

#ifdef DEBUG
#define DEBUG_LOG(message) 
#else
#define DEBUG_LOG()
#endif


   /* initialization should be called before any action with log*/
   void InitLog(const char* pathToFile);
}