#include "PlatformDependentFunctions.h"
#include "StringExtendedFunctions.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineUtility
{
#ifdef _WIN32 // compile only for windows operating system

	uint64_t getProcessMemorySize()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		uint64_t mHeapCapacity = static_cast<uint64_t>(pmc.WorkingSetSize);
		return mHeapCapacity;
	}

	const char *get_module_file_name(HMODULE module)
	{
		size_t size = 1;
		char *buffer;
		for (;;)
		{
			buffer = new char[size + 1];
			DWORD r = GetModuleFileName(module, buffer, size);
			if (r < size && r != 0)
				break;
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				delete buffer;
				size += 64;
			}
			else
				return NULL;
		}
		return buffer;
	}
#elif __linux__ // compile only for linux system operating system

	uint64_t getProcessMemorySize()
	{
		// todo: not implemented yet
		return 0;
	}

	std::string get_module_file_name()
	{
		const auto &path = std::filesystem::canonical("/proc/self/exe");
		const auto &pathToExeStr = path.u8string();
		return pathToExeStr;
	}

#endif

	std::string GetExecutablePath()
	{
		if ("" == sPATH_TO_EXE)
		{
#ifdef _WIN32
			const char *exeFilePathCharPtr = get_module_file_name();
			std::string exeFilePathStr = exeFilePathCharPtr;
			delete exeFilePathCharPtr;
			size_t indexToCurrentDir = LastIndexOf(exeFilePathStr, "\\");
			assert(std::string::npos != indexToCurrentDir);
			sPATH_TO_EXE = exeFilePathStr.substr(0, indexToCurrentDir);
#elif __linux__
			const std::string &fullPath = get_module_file_name();
			const auto indexOfExecutable = LastIndexOf(fullPath, "/");
			assert(indexOfExecutable != std::string::npos);
			sPATH_TO_EXE = fullPath.substr(0, indexOfExecutable + 1);
#endif
		}

		return sPATH_TO_EXE;
	}

	std::string ConvertFromRelativeToAbsolutePath(const std::string &relativePath)
	{
		if ("" == relativePath)
			return relativePath;

		std::string absolutePath = sPATH_TO_EXE;

		int32_t countOfGoBack = 0;
		size_t relativeOffset = 0;
		const std::string &lookForGoBack = "..";

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

		const std::string &relativeTrimmedGoBack = relativePath.substr(relativeOffset);

		while (countOfGoBack != 0)
		{
			size_t lastIndexOfNexDir = EngineUtility::LastIndexOf(absolutePath, "\\");
			absolutePath = absolutePath.substr(0, lastIndexOfNexDir);
			countOfGoBack--;
		}

		absolutePath += relativeTrimmedGoBack;

		return absolutePath;
	}

	std::string FromOsSpecificUrlToGeneral(const std::string &path)
	{
		std::string result = "";
		const auto &splitPathBySlash = Split(path, SLASH);
		for (auto i = 0; i < splitPathBySlash.size(); ++i)
		{
			if (!splitPathBySlash[i].empty())
			{
				if ((i + 1) < splitPathBySlash.size())
				{
					result += splitPathBySlash[i] + GENERAL_SLASH;
				}
				else
				{
					result += splitPathBySlash[i];
				}
			}
		}
		return result;
	}

	std::string FromGeneralUrlToOsSPecific(const std::string &path)
	{
		std::string result = "";
		const auto &splitPathBySlash = Split(path, GENERAL_SLASH);
		for (auto i = 0; i < splitPathBySlash.size(); ++i)
		{
			if (!splitPathBySlash[i].empty())
			{
				if ((i + 1) < splitPathBySlash.size())
				{
					result += splitPathBySlash[i] + SLASH;
				}
				else
				{
					result += splitPathBySlash[i];
				}
			}
		}
		return result;
	}
}