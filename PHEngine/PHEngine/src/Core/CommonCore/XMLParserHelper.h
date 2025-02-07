#pragma once
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <algorithm>
#include <list>
#include <string>

namespace Common {
struct XMLParserHelper {
    using iterator_t = typename std::list<std::string>::const_iterator;

    static iterator_t GetItByNodeName(const std::list<std::string>& fileSource, const std::string& nodeName)
    {
        return std::find_if(fileSource.begin(), fileSource.end(), [&](const std::string& srcNode) {
            return EngineUtility::TrimStart(srcNode) == nodeName;
        });
    }

    static iterator_t GetItByNodeName(const iterator_t& beginIt, const iterator_t& endIt, const std::string& nodeName)
    {
        return std::find_if(
            beginIt, endIt, [&](const std::string& srcNode) { return EngineUtility::TrimStart(srcNode) == nodeName; });
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