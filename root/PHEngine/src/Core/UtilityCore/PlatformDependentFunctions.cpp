#include "PlatformDependentFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "StringExtendedFunctions.h"

#include <chrono>
#include <ctime>
#include <filesystem>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#elif __linux__
#include <cstdlib>
#endif

namespace {
#ifdef __linux__ // compile only for linux OS
std::filesystem::path XdgDir(const char* envVar, const char* fallbackRelToHome)
{
    if (const char* v = std::getenv(envVar); v && *v) {
        return std::filesystem::path(v);
    }
    const char* home = std::getenv("HOME");
    return std::filesystem::path(home ? home : ".") / fallbackRelToHome;
}
#endif
} // namespace

namespace EngineUtility {

std::filesystem::path GetExecutablePath()
{
    static std::filesystem::path sPathToExecutable;
    if (!sPathToExecutable.empty()) {
        return sPathToExecutable;
    }
#ifdef _WIN32
    std::wstring buffer(MAX_PATH, L'\0');
    for (;;) {
        const DWORD length = GetModuleFileNameW(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
        ext_assert(length != 0, "EngineUtility::GetExecutablePath: GetModuleFileNameW failed");
        if (length < buffer.size()) {
            buffer.resize(length);
            break;
        }
        buffer.resize(buffer.size() * 2);
    }
    sPathToExecutable = std::filesystem::path(buffer).parent_path();
#elif __linux__
    sPathToExecutable = std::filesystem::canonical("/proc/self/exe").parent_path();
#endif

    return sPathToExecutable;
}

std::filesystem::path GetDocumentsDir()
{
    std::filesystem::path result;
#ifdef _WIN32
    PWSTR raw = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &raw))) {
        result = std::filesystem::path(raw);
    }
    CoTaskMemFree(raw);
#elif __linux__
    result = XdgDir("XDG_DOCUMENTS_DIR", "Documents");
#endif
    return result;
}

std::filesystem::path GetAppDataDir()
{
    std::filesystem::path result;
#ifdef _WIN32
    PWSTR raw = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, nullptr, &raw))) {
        result = std::filesystem::path(raw);
    }
    CoTaskMemFree(raw);
#elif __linux__
    result = XdgDir("XDG_DATA_HOME", ".local/share");
#endif
    return result;
}

std::filesystem::path ConvertFromRelativeToAbsolutePath(const std::filesystem::path& relativePath)
{
    if (relativePath.empty())
        return relativePath;

    return (GetExecutablePath() / relativePath).lexically_normal();
}

std::string PathToUtf8(const std::filesystem::path& path)
{
    // In C++20 u8string() returns std::u8string (char8_t), not std::string.
    // The underlying bytes are identical to UTF-8 char bytes, so the range
    // constructor is the safest way to obtain a std::string.
    const auto u8 = path.u8string();
    return {u8.begin(), u8.end()};
}

std::string FromOsSpecificUrlToGeneral(const std::string& path)
{
    std::string result = "";
    const auto& splitPathBySlash = Split(path, SLASH);
    for (size_t i = 0; i < splitPathBySlash.size(); ++i) {
        if (!splitPathBySlash[i].empty()) {
            if ((i + 1) < splitPathBySlash.size()) {
                result += splitPathBySlash[i] + PIPE;
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
    const auto& splitPathBySlash = Split(path, PIPE);
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