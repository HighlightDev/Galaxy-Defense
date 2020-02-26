#include "StringExtendedFunctions.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace EngineUtility
{

	bool StartsWith(const std::string& sourceStr, const std::string& lookfor)
	{
		return sourceStr.find(lookfor) == 0;
	}

	size_t IndexOf(const std::string& sourceStr, const std::string& lookfor, size_t offset)
	{
		size_t result = std::string::npos;
		typename std::string::size_type location = sourceStr.find(lookfor, offset);
		if (location != std::string::npos)
			result = location;

		return result;
	}

	size_t LastIndexOf(const std::string& sourceStr, const std::string& lookfor, size_t offset)
	{
		size_t index = std::string::npos;
		size_t new_offset = 0;
		do
		{
			new_offset = IndexOf(sourceStr, lookfor, new_offset);
			if (new_offset != std::string::npos)
			{
				index = new_offset;
				new_offset += lookfor.size();
			}
		} while (new_offset != std::string::npos);

		return index;
	}

	std::string TrimStart(const std::string& sourceStr)
	{
		std::string str = sourceStr;
		auto trim = [](std::string& s) -> void
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
				[](int32_t ch) {
				return ! std::isspace(ch); }
			));
		};

		trim(str);
		return str;
	}

	std::string TrimEnd(const std::string& sourceStr)
	{
		std::string str = sourceStr;
		auto trim = [](std::string& s) -> void
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
				return !std::isspace(ch);
			}).base(), s.end());
		};

		trim(str);
		return str;
	}

	std::vector<std::string> Split(const std::string& source, char splitChar)
	{
		std::stringstream test(source);
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(test, segment, splitChar))
		{
			seglist.push_back(segment);
		}
		return seglist;
	}

}