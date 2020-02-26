#include "PlatformDependentFunctions.h"
#include "StringExtendedFunctions.h"

namespace EngineUtility
{
#ifdef _WIN32 // compile only for windows operating system

#include <windows.h>

	char * get_module_file_name(HMODULE hModule) {
		size_t size = 1;
		char * buffer;
		for (; ; ) {
			buffer = new char[size + 1];
			DWORD r = GetModuleFileName(hModule, buffer, size);
			if (r < size && r != 0) break;
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				delete buffer;
				size += 64;
			}
			else return NULL;
		}
		return buffer;
	}
#elif __linux__  // compile only for linux system operating system

#endif

	std::string GetExecutablePath()
	{
		std::string exeFilePath = std::move(std::string(get_module_file_name()));
		size_t indexToCurrentDir = LastIndexOf(exeFilePath, "\\");
		std::string currentDirPath = exeFilePath.substr(0, indexToCurrentDir);

		return std::move(currentDirPath);
	}

	std::string ConvertFromRelativeToAbsolutePath(const std::string& relativePath)
	{
      if ("" == relativePath)
         return relativePath;

		std::string pathToExe = std::move(EngineUtility::GetExecutablePath());
		std::string absolutePath = pathToExe;

		int32_t countOfGoBack = 0;
		std::string relativeTrimmedGoBack;

		size_t relativeOffset = 0;
		const std::string& lookForGoBack = "..";

		size_t new_offset = 0;
		do
		{
			new_offset = IndexOf(relativePath, lookForGoBack, relativeOffset);
			if (new_offset != std::string::npos)
			{
				relativeOffset = new_offset + lookForGoBack.size();
				countOfGoBack++;
			}
		} while (new_offset != std::string::npos);


		relativeTrimmedGoBack = relativePath.substr(relativeOffset);

		while (countOfGoBack != 0)
		{
			size_t lastIndexOfNexDir = EngineUtility::LastIndexOf(absolutePath, "\\");
			absolutePath = absolutePath.substr(0, lastIndexOfNexDir);
			countOfGoBack--;
		}

		absolutePath += relativeTrimmedGoBack;

		return std::move(absolutePath);
	}
}