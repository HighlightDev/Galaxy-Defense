#include "StringExtendedFunctions.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace EngineUtility {

bool StartsWith(const std::string& sourceStr, const std::string& lookfor)
{
    return sourceStr.find(lookfor) == 0;
}

bool HasSubstringPresence(const std::string& sourceStr, const std::string& lookfor)
{
    return sourceStr.find(lookfor) != std::string::npos;
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
    do {
        new_offset = IndexOf(sourceStr, lookfor, new_offset);
        if (new_offset != std::string::npos) {
            index = new_offset;
            new_offset += lookfor.size();
        }
    } while (new_offset != std::string::npos);

    return index;
}

std::string TrimStart(const std::string& sourceStr)
{
    std::string str = sourceStr;
    auto trim = [](std::string& s) -> void {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int32_t ch) { return !std::isspace(ch); }));
    };

    trim(str);
    return str;
}

std::string TrimEnd(const std::string& sourceStr)
{
    std::string str = sourceStr;
    auto trim = [](std::string& s) -> void {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
    };

    trim(str);
    return str;
}

std::string Trim(const std::string& source)
{
    return TrimEnd(TrimStart(source));
}

std::vector<std::string> Split(const std::string& source, char splitChar)
{
    std::stringstream test(source);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(test, segment, splitChar)) {
        seglist.push_back(segment);
    }
    return seglist;
}

std::string ToLower(const std::string& source)
{
    std::string result = source;
    std::transform(source.begin(), source.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });

    return result;
}

std::string RemoveAll(const std::string& source, const char symbol)
{
    std::string result = source;
    const auto newIt = std::remove_if(result.begin(), result.end(), [&](const auto& symb) { return symb == symbol; });
    result.erase(newIt, result.end());
    return result;
}

int32_t Utf8_To_Unicode(const std::string& utf8_code)
{
    const size_t utf8_size = utf8_code.length();
    int32_t result_unicode = 0;

    for (unsigned p = 0; p < utf8_size; ++p) {
        const int32_t bit_count = (p ? 6 : 8 - utf8_size - (utf8_size == 1 ? 0 : 1)),
                      shift = (p < utf8_size - 1 ? (6 * (utf8_size - p - 1)) : 0);

        for (int k = 0; k < bit_count; ++k)
            result_unicode += ((utf8_code[p] & (1 << k)) << shift);
    }

    return result_unicode;
}

std::vector<std::string> ExtractUtf8FromUnicodeString(const std::string& unicodeString)
{
    std::vector<std::string> utf8_vector;
    for (size_t i = 0; i < unicodeString.length();) {
        int32_t cplen = 1;
        if ((unicodeString[i] & 0xf8) == 0xf0) {
            cplen = 4;
        } else if ((unicodeString[i] & 0xf0) == 0xe0) {
            cplen = 3;
        } else if ((unicodeString[i] & 0xe0) == 0xc0) {
            cplen = 2;
        }
        if ((i + cplen) > unicodeString.length()) {
            cplen = 1;
        }

        utf8_vector.push_back(unicodeString.substr(i, cplen));
        i += cplen;
    }

    return utf8_vector;
}
} // namespace EngineUtility