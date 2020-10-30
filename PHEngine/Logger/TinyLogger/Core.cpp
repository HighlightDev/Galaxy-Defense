#include "Core.h"

namespace TinyLogger
{
   LogMessage::LogMessage(std::initializer_list<std::string> messages)
      : mLogs(messages)
   {
   }

   std::string LogMessage::GetLog() const
   {
      std::string result;

      std::for_each(mLogs.begin(), mLogs.end(), [&](const std::string& logArg) { result += "  " + logArg; });

      return result;
   }

   Logger::Logger()
      : mLogThreadStarted(false)
   {

   }

   Logger* Logger::GetInstance_()
   {
      static Logger loggerInstance;
      return &loggerInstance;
   }

   void Logger::AddLoggerClient(LoggerClientBase* clientBase)
   {
      mLoggerClients.push_back(clientBase);
   }

   void Logger::StartLogThread()
   {
      if (!mLogThreadStarted)
      {
         mLogThread = std::thread(std::bind(&Logger::WriteLogMessagesToFile, this));
         mLogThread.detach();
         mLogThreadStarted = true;
      }
   }

   void Logger::EnqueuLogMessage(LogMessage&& message)
   {
      std::lock_guard<std::mutex> lock(mWriteToFileMutex);
      mMessageQueue.emplace(std::move(message));
   }

   void Logger::WriteLogMessagesToFile()
   {
      while (true)
      {
         std::unique_lock<std::mutex> uLock(mWriteToFileMutex);
         while (!mMessageQueue.empty())
         {
            auto message = mMessageQueue.front();
            const std::string& log = message.GetLog();
            mMessageQueue.pop();

            std::for_each(mLoggerClients.begin(), mLoggerClients.end(), [&](LoggerClientBase* client) { client->WriteLog(log); });
         }
         uLock.unlock();
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
   }
}
