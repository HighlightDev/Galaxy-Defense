#include "NavigationPathBuilder.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
    void NavigationPathBuilder::AddPath(const std::string &pathName, const Path &pathSegment)
    {
        assert(!mPaths.count(pathName));
        mPaths[pathName] = pathSegment;
    }

    Path NavigationPathBuilder::GetPathByName(const std::string &name) const
    {
        assert(mPaths.count(name));
        return mPaths.at(name);
    }

    bool NavigationPathBuilder::TryToRemovePathByName(const std::string &name)
    {
        if (mPaths.count(name))
        {
            mPaths.erase(name);
            return true;
        }
        return false;
    }

    const std::unordered_map<std::string, Path>& NavigationPathBuilder::GetPaths() const
    {
        return mPaths;
    }

    std::unordered_map<std::string, Path>& NavigationPathBuilder::GetPaths()
    {
        return mPaths;
    }
}
