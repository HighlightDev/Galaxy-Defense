#include "Core.h"

#include <algorithm>

namespace TinyLogger
{
   LogMessage::LogMessage(std::initializer_list<std::string> messages)
      : mLogs(messages)
   {
   }

   LogMessage::LogMessage(std::vector<std::string> messages)
      : mLogs(std::move(messages)) 
   {
   }

   std::string LogMessage::GetLog() const
   {
      std::string result;

      std::for_each(mLogs.begin(), mLogs.end(), [&](const std::string& logArg) { result += "  " + logArg; });

      return result;
   }

   LoggerServer::LoggerServer()
      : mLogThreadStarted(false)
   {

   }

   LoggerServer* LoggerServer::GetInstance_()
   {
      static LoggerServer loggerInstance;
      return &loggerInstance;
   }

   void LoggerServer::AddLoggerClient(LoggerClientBase* clientBase)
   {
      mLoggerClients.push_back(clientBase);
   }

   void LoggerServer::StartLogThread()
   {
      if (!mLogThreadStarted)
      {
         mLogThread = std::thread(std::bind(&LoggerServer::WriteLogMessages, this));
         mLogThread.detach();
         mLogThreadStarted = true;
      }
   }

   void LoggerServer::EnqueuLogMessage(LogMessage&& message)
   {
      std::lock_guard<std::mutex> lock(mWriteToFileMutex);
      mMessageQueue.emplace(message);
   }

   std::string LoggerServer::ConcatMessages()
   {
      std::string result = "";

      while (!mMessageQueue.empty())
      {
         auto message = mMessageQueue.front();
         const std::string& log = message.GetLog();
         result += log + "\n";
         mMessageQueue.pop();
      }

      return result;
   }

   void LoggerServer::WriteLogMessages()
   {
      while (true)
      {
         std::unique_lock<std::mutex> uLock(mWriteToFileMutex);
         if (!mMessageQueue.empty())
         {
            const std::string& log = ConcatMessages();
            if ("" != log)
            {
               std::for_each(mLoggerClients.begin(), mLoggerClients.end(), [&](LoggerClientBase* client) { client->WriteLog(log); });
            }
         }
         uLock.unlock();
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
   }
}
