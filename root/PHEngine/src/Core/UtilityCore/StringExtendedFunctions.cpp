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
    return std::string(TrimStartView(sourceStr));
}

std::string_view TrimStartView(std::string_view sourceStr)
{
    const auto trimmedIndex = sourceStr.find_first_not_of(" \t\r\n\v\f");
    if (std::string_view::npos != trimmedIndex) {
        return sourceStr.substr(trimmedIndex);
    }
    return std::string_view{};
}

std::string TrimEnd(const std::string& sourceStr)
{
    return std::string(TrimEndView(sourceStr));
}

std::string_view TrimEndView(std::string_view sourceStr)
{
    const auto lastTrimmedIndex = sourceStr.find_last_not_of(" \t\r\n\v\f");
    if (std::string_view::npos != lastTrimmedIndex) {
        return sourceStr.substr(0, lastTrimmedIndex + 1);
    }
    return std::string_view{};
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

std::string Join(const std::vector<std::string>& source, char joinChar)
{
    std::string result;
    for (size_t i = 0; i < source.size(); ++i) {
        result += source[i];
        if (i != source.size() - 1) {
            result += joinChar;
        }
    }
    return result;
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

std::vector<uint32_t> Utf8_To_Unicode(const std::string& utf8Str)
{
    // Convert UTF-8 string to a vector of Unicode codepoints
    std::vector<uint32_t> codepoints;
    for (size_t i = 0; i < utf8Str.size();) {
        uint32_t cp = 0;
        unsigned char c = utf8Str[i];
        if (c < 0x80) { // 1-byte sequence
            cp = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) { // 2-byte sequence
            cp = ((c & 0x1F) << 6) | (utf8Str[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3-byte sequence
            cp = ((c & 0x0F) << 12) | ((utf8Str[i + 1] & 0x3F) << 6) | (utf8Str[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4-byte sequence
            cp = ((c & 0x07) << 18) | ((utf8Str[i + 1] & 0x3F) << 12) | ((utf8Str[i + 2] & 0x3F) << 6) | (utf8Str[i + 3] & 0x3F);
            i += 4;
        } else {
            // Invalid UTF-8, skip
            i += 1;
            continue;
        }
        codepoints.emplace_back(cp);
    }
    return codepoints;
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