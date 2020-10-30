#pragma once
#include <vector>
#include <queue>
#include <string>
#include <initializer_list>
#include <mutex>
#include <thread>

#include "LoggerClient.h"

namespace TinyLogger
{
   struct LogMessage
   {
   private:

      std::vector<std::string> mLogs;

   public:

      LogMessage(std::initializer_list<std::string> messages);
   
      std::string GetLog() const;
   };

   class Logger
   {
      std::mutex mWriteToFileMutex;
      std::thread mLogThread;

      std::queue<LogMessage> mMessageQueue;
      std::vector<LoggerClientBase*> mLoggerClients;

      bool mLogThreadStarted;

      Logger();

   public:

      static Logger* GetInstance_();

      void AddLoggerClient(LoggerClientBase* clientBase);

      void StartLogThread();

      void EnqueuLogMessage(LogMessage&& message);

   private:

      void WriteLogMessagesToFile();
   };
}

