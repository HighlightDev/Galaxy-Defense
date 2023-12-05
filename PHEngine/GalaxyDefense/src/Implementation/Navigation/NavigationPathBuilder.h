#pragma once

#include "Path.h"

#include <unordered_map>

namespace Game
{
    class NavigationPathBuilder
    {
        std::unordered_map<std::string, Path> mPaths;

    public:
        void AddPath(const std::string& pathName, const Path &pathSegment);

        Path GetPathByName(const std::string& name) const;

        bool TryToRemovePathByName(const std::string& name);

        const std::unordered_map<std::string, Path>& GetPaths() const;

        std::unordered_map<std::string, Path>& GetPaths();
    };
}
