#pragma once
#include "LoggerClient.h"

#include <atomic>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace TinyLogger {
struct LogMessage {
private:
    std::vector<std::string> mLogs;

public:
    LogMessage(std::vector<std::string>&& messages);

    std::string GetLog() const;
};

class LoggerServer {
    std::mutex mWriteToFileMutex;
    std::thread mLogThread;
    std::atomic<bool> mIsThreadRunning{false};

    std::queue<LogMessage> mMessageQueue;
    std::vector<std::shared_ptr<LoggerClientBase>> mLoggerClients;

    LoggerServer();

public:
    static LoggerServer* GetInstance_();

    void AddLoggerClient(const std::shared_ptr<LoggerClientBase>& clientBase);

    void StartLogThread();

    void StopLogThread();

    void EnqueuLogMessage(LogMessage message);

private:
    void WriteLogMessage();

    void UpdateLoggerMainLoop();

    std::string ConcatMessages();
};
} // namespace TinyLogger
