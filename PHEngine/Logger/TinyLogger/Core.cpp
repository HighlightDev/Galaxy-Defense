#include "Core.h"

#include <algorithm>
#include <functional>

namespace TinyLogger {
LogMessage::LogMessage(std::vector<std::string>&& messages)
    : mLogs(std::move(messages))
{
}

std::string LogMessage::GetLog() const
{
    std::string result;

    std::for_each(mLogs.begin(), mLogs.end(), [&](const std::string& logArg) { result += " " + logArg; });

    return result;
}

LoggerServer::LoggerServer()
    : mMessageQueue()
    , mLoggerClients()
{
}

LoggerServer* LoggerServer::GetInstance_()
{
    static LoggerServer loggerInstance;
    return &loggerInstance;
}

void LoggerServer::AddLoggerClient(const std::shared_ptr<LoggerClientBase>& clientBase)
{
    mLoggerClients.push_back(clientBase);
}

void LoggerServer::StartLogThread()
{
    if (!mIsThreadRunning.load(std::memory_order::memory_order_seq_cst)) {
        mIsThreadRunning.store(true, std::memory_order::memory_order_seq_cst);
        mLogThread = std::thread(std::bind(&LoggerServer::UpdateLoggerMainLoop, this));
    }
}

void LoggerServer::StopLogThread()
{
    if (mIsThreadRunning.load(std::memory_order::memory_order_seq_cst)) {
        mIsThreadRunning.store(false, std::memory_order::memory_order_seq_cst);
        mLogThread.join();
    }
    WriteLogMessage();
}

void LoggerServer::EnqueuLogMessage(LogMessage message)
{
    std::lock_guard<std::mutex> lock(mWriteToFileMutex);
    mMessageQueue.emplace(std::move(message));
}

std::string LoggerServer::ConcatMessages()
{
    std::string result = "";

    while (!mMessageQueue.empty()) {
        auto message = mMessageQueue.front();
        const std::string& log = message.GetLog();
        result += log + "\n";
        mMessageQueue.pop();
    }

    return result;
}

void LoggerServer::WriteLogMessage()
{
    std::lock_guard<std::mutex> writeLock(mWriteToFileMutex);
    if (!mMessageQueue.empty()) {
        const std::string& log = ConcatMessages();
        if ("" != log) {
            std::for_each(mLoggerClients.begin(), mLoggerClients.end(), [&](const auto& client) { client->WriteLog(log); });
        }
    }
}

void LoggerServer::UpdateLoggerMainLoop()
{
    while (mIsThreadRunning.load(std::memory_order::memory_order_seq_cst)) {
        WriteLogMessage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
} // namespace TinyLogger
