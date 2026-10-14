#pragma once

#include "Path.h"

#include <string>
#include <unordered_map>

namespace Game {
class NavigationPathBuilder {
    std::unordered_map<std::string, Path> mPaths;

    std::unordered_multimap<std::string, std::pair<std::string, Path>> mExtendedPaths;

public:
    void AddPath(const std::string& pathName, const Path& pathSegment);

    void ExtendPath(const std::string& pathName, const int32_t eachSideExtraPathCount);

    Path GetPathByName(const std::string& name) const;

    bool TryToRemovePathByName(const std::string& name);

    const std::unordered_map<std::string, Path>& GetPaths() const;

    std::unordered_map<std::string, Path>& GetPaths();

    std::unordered_multimap<std::string, std::pair<std::string, Path>>& GetExtendedPaths();

    const std::unordered_multimap<std::string, std::pair<std::string, Path>>& GetExtendedPaths() const;
};
} // namespace Game
