#pragma once

#include <fstream>
#include <string>
#include <iostream>

namespace TinyLogger
{

   class LoggerClientBase
   {
   public:
      virtual void WriteLog(const std::string& message) = 0;
   };

   class LoggerClientConsole
      : public LoggerClientBase
   {
   public:
      virtual void WriteLog(const std::string& message) override
      {
         std::cout << message << std::endl;
      }
   };

   class LoggerClientFile
      : public LoggerClientBase
   {
      bool bClearFileAtStart = true;

      void WriteToFile(const std::string message)
      {
         std::ofstream stream;

         std::ios_base::openmode openFileFlag = std::ios_base::app;

         if (bClearFileAtStart)
         {
            openFileFlag = std::ios_base::out;
            bClearFileAtStart = false;
         }

         stream.open("DebugTrace.log", openFileFlag);

         stream << message;
      }

   public:

      virtual void WriteLog(const std::string& message) override
      {
         WriteToFile(message);
      }

   };

}
