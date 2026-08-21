#pragma once

#include <string>

namespace TinyLogger {

class LoggerClientBase {

public:
    virtual void WriteLog(const std::string& message) = 0;
};

class LoggerClientConsole : public LoggerClientBase {

public:
    void WriteLog(const std::string& message) override;
};

class LoggerClientFile : public LoggerClientBase {

    std::string mPathToOutput;

    bool bClearFileAtStart{true};

    void WriteToFile(const std::string& message);

public:
    explicit LoggerClientFile(const std::string& pathToOutput);

    void WriteLog(const std::string& message) override;
};

} // namespace TinyLogger
