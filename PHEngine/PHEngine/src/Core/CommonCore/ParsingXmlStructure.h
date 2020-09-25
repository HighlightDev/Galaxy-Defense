#pragma once
#include <string>
#include <list>
#include <algorithm>

#include "Core/UtilityCore/StringExtendedFunctions.h"

namespace Common
{
   using iterator_t = typename std::list<std::string>::const_iterator;

   iterator_t GetItByNodeName(const std::list<std::string>& fileSource, const std::string& nodeName)
   {
      return std::find_if(fileSource.begin(), fileSource.end(), [&](const std::string& srcNode) { return EngineUtility::TrimStart(srcNode) == nodeName; });
   }

   iterator_t GetItByNodeName(const iterator_t& beginIt, const iterator_t& endIt, const std::string& nodeName)
   {
      return std::find_if(beginIt, endIt, [&](const std::string& srcNode) { return EngineUtility::TrimStart(srcNode) == nodeName; });
   }

   std::string GetPropertyNodeByName(const std::string& trimmedNodeStr, const std::string& propName)
   {
      std::string result;

      const size_t nameStartIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"") + 1;
      const size_t nameEndIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"", nameStartIndex);
      result = trimmedNodeStr.substr(nameStartIndex, nameEndIndex - nameStartIndex);

      return result;
   }
}