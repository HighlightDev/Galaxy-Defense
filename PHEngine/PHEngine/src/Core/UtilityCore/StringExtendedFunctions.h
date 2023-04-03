#pragma once

#include <string>
#include <vector>

namespace EngineUtility
{

	bool StartsWith(const std::string &source, const std::string &lookfor);
	
	bool HasSubstringPresence(const std::string& sourceStr, const std::string& lookfor);

	size_t IndexOf(const std::string &source, const std::string &lookfor, size_t offset = 0);

	size_t LastIndexOf(const std::string &source, const std::string &lookfor, size_t offset = 0);

	std::string TrimStart(const std::string &source);

	std::string TrimEnd(const std::string &source);

	std::string Trim(const std::string &source);

	std::vector<std::string> Split(const std::string &source, char splitChar);

	std::string ToLower(const std::string &source);

	std::string RemoveAll(const std::string &source, const char symbol);

	int32_t Utf8_To_Unicode(const std::string &utf8_code);

	std::vector<std::string> ExtractUtf8FromUnicodeString(const std::string &unicodeString);
}