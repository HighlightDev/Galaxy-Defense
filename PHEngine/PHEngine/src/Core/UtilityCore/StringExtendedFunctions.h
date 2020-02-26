#pragma once

#include <string>
#include <vector>

namespace EngineUtility
{

	bool StartsWith(const std::string& source, const std::string& lookfor);

	size_t IndexOf(const std::string& source, const std::string& lookfor, size_t offset = 0);

	size_t LastIndexOf(const std::string& source, const std::string& lookfor, size_t offset = 0);

	std::string TrimStart(const std::string& source);

	std::string TrimEnd(const std::string& source);

	std::vector<std::string> Split(const std::string& source, char splitChar);
}