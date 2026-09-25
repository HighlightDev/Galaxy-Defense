#include "PlatformDependentFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "StringExtendedFunctions.h"

#include <chrono>
#include <ctime>

namespace EngineUtility {
#ifdef _WIN32 // compile only for windows operating system

const char* get_module_file_name(HMODULE module)
{
    DWORD size = 1;
    char* buffer;
    for (;;) {
        buffer = new char[size + 1];
        DWORD r = GetModuleFileName(module, buffer, size);
        if (r < size && r != 0)
            break;
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            delete buffer;
            size += 64;
        } else
            return NULL;
    }
    return buffer;
}
#elif __linux__ // compile only for linux system operating system

std::string get_module_file_name()
{
    const auto& path = std::filesystem::canonical("/proc/self/exe");
    const auto& pathToExeStr = path.string();
    return pathToExeStr;
}

#endif

std::string GetExecutablePath()
{
    if (sPathToExecutable.empty()) {
#ifdef _WIN32
        const char* exeFilePathCharPtr = get_module_file_name();
        std::string exeFilePathStr = exeFilePathCharPtr;
        delete exeFilePathCharPtr;
        size_t indexToCurrentDir = LastIndexOf(exeFilePathStr, "\\");
        ext_assert(std::string::npos != indexToCurrentDir, "EngineUtility::GetExecutablePath: Invalid executable path");
        sPathToExecutable = exeFilePathStr.substr(0, indexToCurrentDir);
#elif __linux__
        const std::string& fullPath = get_module_file_name();
        const auto indexOfExecutable = LastIndexOf(fullPath, "/");
        ext_assert(indexOfExecutable != std::string::npos, "EngineUtility::GetExecutablePath: Invalid executable path");
        sPathToExecutable = fullPath.substr(0, indexOfExecutable + 1);
#endif
    }

    return sPathToExecutable;
}

std::string ConvertFromRelativeToAbsolutePath(const std::string& relativePath)
{
    if ("" == relativePath)
        return relativePath;

    std::string absolutePath = sPathToExecutable;

    int32_t countOfGoBack = 0;
    size_t relativeOffset = 0;
    const std::string& lookForGoBack = "..";

    size_t new_offset = 0;
    do {
        new_offset = IndexOf(relativePath, lookForGoBack, relativeOffset);
        if (new_offset != std::string::npos) {
            relativeOffset = new_offset + lookForGoBack.size();
            countOfGoBack++;
        }
    } while (new_offset != std::string::npos);

    const std::string& relativeTrimmedGoBack = relativePath.substr(relativeOffset);

    while (countOfGoBack != 0) {
        size_t lastIndexOfNexDir = EngineUtility::LastIndexOf(absolutePath, "\\");
        absolutePath = absolutePath.substr(0, lastIndexOfNexDir);
        countOfGoBack--;
    }

    absolutePath += relativeTrimmedGoBack;

    return absolutePath;
}

std::string FromOsSpecificUrlToGeneral(const std::string& path)
{
    std::string result = "";
    const auto& splitPathBySlash = Split(path, SLASH);
    for (size_t i = 0; i < splitPathBySlash.size(); ++i) {
        if (!splitPathBySlash[i].empty()) {
            if ((i + 1) < splitPathBySlash.size()) {
                result += splitPathBySlash[i] + GENERAL_SLASH;
            } else {
                result += splitPathBySlash[i];
            }
        }
    }
    return result;
}

std::string FromGeneralUrlToOsSpecific(const std::string& path)
{
    std::string result = "";
    const auto& splitPathBySlash = Split(path, GENERAL_SLASH);
    for (size_t i = 0; i < splitPathBySlash.size(); ++i) {
        if (!splitPathBySlash[i].empty()) {
            if ((i + 1) < splitPathBySlash.size()) {
                result += splitPathBySlash[i] + SLASH;
            } else {
                result += splitPathBySlash[i];
            }
        }
    }
    return result;
}

std::string GetLocalTimeStr()
{
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#ifdef _WIN32
    char timeFileWasChanged[26];
    struct tm timeinfo;
    localtime_s(&timeinfo, &currentTime);
    asctime_s(timeFileWasChanged, sizeof timeFileWasChanged, &timeinfo);
#elif __linux__
    std::asctime(std::localtime(&currentTime));
    const std::string& timeFileWasChanged = std::asctime(std::localtime(&currentTime));
#endif
    return timeFileWasChanged;
}
} // namespace EngineUtility