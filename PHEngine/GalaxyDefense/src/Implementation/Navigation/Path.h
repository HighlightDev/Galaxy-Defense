#pragma once

#include "PathSegment.h"

#include <vector>

namespace Game
{
    class Path
    {
        std::vector<PathSegment> mPathSegments;

        std::vector<glm::vec3> mCachedRoutePoints;

    public:
        void AppendPathSegmentToTheEnd(const PathSegment &pathSegment);

        const std::vector<PathSegment> &GetPathSegments() const;

        const std::vector<glm::vec3> &GetRoutePoints();

        glm::vec3 GetRouteFirstPoint() const;
    };
}
