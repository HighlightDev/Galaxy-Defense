#pragma once

#include <string>

namespace EngineUtility
{
   static std::string sPATH_TO_EXE = "";

   uint64_t getProcessMemorySize();

#ifdef _WIN32 // compile only for windows operating system
   const char *get_module_file_name(HMODULE module = NULL);
#elif __linux__ // compile only for linux system operating system
   std::string get_module_file_name();
#endif

   std::string GetExecutablePath();

   std::string ConvertFromRelativeToAbsolutePath(const std::string &relativePath);

}
