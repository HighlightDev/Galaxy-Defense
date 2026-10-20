#pragma once
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <algorithm>
#include <list>
#include <string>
#include <string_view>

namespace Common {
struct XMLParserHelper {
    using iterator_t = typename std::list<std::string>::const_iterator;

    static iterator_t GetItByNodeName(const std::list<std::string>& fileSource, std::string_view nodeName)
    {
        return std::find_if(fileSource.begin(), fileSource.end(), [&](std::string_view srcNode) {
            return EngineUtility::TrimStartView(srcNode) == nodeName;
        });
    }

    static iterator_t GetItByNodeName(const iterator_t& beginIt, const iterator_t& endIt, std::string_view nodeName)
    {
        return std::find_if(
            beginIt, endIt, [&](std::string_view srcNode) { return EngineUtility::TrimStartView(srcNode) == nodeName; });
    }

    static std::string GetPropertyNodeAfterColon(const std::string& trimmedNodeStr)
    {
        std::string result;

        const size_t nameStartIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"") + 1;
        const size_t nameEndIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"", nameStartIndex);
        result = trimmedNodeStr.substr(nameStartIndex, nameEndIndex - nameStartIndex);

        return result;
    }

    static std::string GetSubstringInsideBrackets(const std::string& nodeStr)
    {

        const size_t startBrackets = EngineUtility::IndexOf(nodeStr, "[") + 1;
        const size_t endBrackets = EngineUtility::IndexOf(nodeStr, "]", startBrackets);
        return nodeStr.substr(startBrackets, endBrackets - startBrackets);
    }
};
} // namespace Common