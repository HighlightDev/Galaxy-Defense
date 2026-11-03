#pragma once

#include <stdint.h>

#include <cstdint>
#include <filesystem>
#include <string>

#ifdef _WIN32
constexpr auto SLASH = '\\';
#elif __linux__
constexpr auto SLASH = '/';
#endif
constexpr auto PIPE = '|';

namespace EngineUtility {
std::filesystem::path GetExecutablePath();

std::filesystem::path GetDocumentsDir();

std::filesystem::path GetAppDataDir();

std::filesystem::path ConvertFromRelativeToAbsolutePath(const std::filesystem::path& relativePath);

// std::filesystem::path::string() uses the system ANSI code page on Windows and throws
// std::system_error when the path contains characters outside that code page.
// This helper always produces a valid UTF-8 std::string regardless of the locale.
std::string PathToUtf8(const std::filesystem::path& path);

std::string FromOsSpecificUrlToGeneral(const std::string& path);

std::string FromGeneralUrlToOsSpecific(const std::string& path);

std::string GetLocalTimeStr();
} // namespace EngineUtility
