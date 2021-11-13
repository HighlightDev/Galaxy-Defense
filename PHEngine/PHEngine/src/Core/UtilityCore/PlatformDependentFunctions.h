#pragma once

#include <string>

namespace EngineUtility
{

#ifdef _WIN32 // compile only for windows operating system

#include <windows.h>
#include <wchar.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

   uint64_t getProcessMemorySize();

	char * get_module_file_name(HMODULE hModule = NULL);
#elif __linux__  // compile only for linux system operating system

#endif

   std::string GetExecutablePath();

	std::string ConvertFromRelativeToAbsolutePath(const std::string& relativePath);

}
