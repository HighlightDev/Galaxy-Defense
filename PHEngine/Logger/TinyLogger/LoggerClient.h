#pragma once

#include <string>

namespace TinyLogger
{

   class LoggerClientBase
   {
   public:
      virtual void WriteLog(const std::string& message) = 0;
   };

#ifdef WIN32
#include <iostream>

   class LoggerClientConsole
      : public LoggerClientBase
   {
   public:
      virtual void WriteLog(const std::string& message) override
      {
         std::cout << message << std::endl;
      }
   };
#endif

}
