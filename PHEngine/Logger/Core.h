#pragma once
#include <vector>
#include <queue>
#include <string>
#include <initializer_list>
#include <mutex>

namespace Log
{
   struct LogMessage
   {
   private:

      std::vector<std::string> mLogs;

   public:

      LogMessage(std::initializer_list<std::string> messages)
         : mLogs(messages)
      {
      }
   
      std::string GetLog() const
      {
         std::string result;

         std::for_each(mLogs.begin(), mLogs.end(), [&](const std::string& logArg) { result += "  " + logArg; });

         return result;
      }
   };

   class Logger
   {
      std::queue<LogMessage> mMessageQueue;

      static Logger* mInstance;

      std::mutex mWriteToFileMutex;

      Logger() {}

   public:

      static Logger* GetInstance_()
      {
         if (!mInstance)
            mInstance = new Logger();

         return mInstance;
      }

      void EnqueuLogMessage(LogMessage&& message)
      {
         std::lock_guard<std::mutex> lock(mWriteToFileMutex);
         mMessageQueue.emplace(std::move(message));
      }

      void WriteLogMessagesToFile()
      {
         std::unique_lock<std::mutex> uLock(mWriteToFileMutex);
         while (!mMessageQueue.empty())
         {
            auto message = mMessageQueue.front();
            std::string log = message.GetLog();
            mMessageQueue.pop();
         }
         uLock.release();

         std::this_thread::sleep_for(std::chrono::seconds(2));
      }
   };
}

