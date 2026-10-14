#include "LoggerClient.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace TinyLogger {

void LoggerClientConsole::WriteLog(const std::string& message)
{
    std::cout << message << std::endl;
}

void LoggerClientFile::WriteToFile(const std::string& message)
{
    if (mPathToOutput.size() <= 0) {
        std::cout << "Path to output must be not empty!" << std::endl;
        throw "Path to output must be not empty!";
    }
    if (not std::filesystem::is_directory(mPathToOutput)) {
        std::cout << "Path to output must be directory! Path: " << mPathToOutput << std::endl;
        throw "Path to output must be directory!";
    }

    std::ofstream stream;

    std::ios_base::openmode openFileFlag = std::ios_base::app;

    if (bClearFileAtStart) {
        openFileFlag = std::ios_base::out;
        bClearFileAtStart = false;
    }

    stream.open(mPathToOutput + "/DebugTrace.log", openFileFlag);

    stream << message;
}

LoggerClientFile::LoggerClientFile(const std::string& pathToOutput)
    : LoggerClientBase()
    , mPathToOutput(pathToOutput)
{
}

void LoggerClientFile::WriteLog(const std::string& message)
{
    WriteToFile(message);
}

} // namespace TinyLogger
