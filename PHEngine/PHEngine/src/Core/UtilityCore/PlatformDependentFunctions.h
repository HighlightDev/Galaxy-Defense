#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>

#ifdef _WIN32
#include <windows.h>
constexpr auto SLASH = '\\';
#elif __linux__
#include <filesystem>
#define SLASH '/'
#endif

#define GENERAL_SLASH '|'

namespace EngineUtility {
static std::string sPATH_TO_EXE = "";

#ifdef _WIN32 // compile only for windows operating system
const char* get_module_file_name(HMODULE module = NULL);
#elif __linux__ // compile only for linux system operating system
std::string get_module_file_name();
#endif

std::string GetExecutablePath();

std::string ConvertFromRelativeToAbsolutePath(const std::string& relativePath);

std::string FromOsSpecificUrlToGeneral(const std::string& path);

std::string FromGeneralUrlToOsSpecific(const std::string& path);

std::string GetLocalTimeStr();
} // namespace EngineUtility
