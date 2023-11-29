#include "NavigationPathBuilder.h"

#include <algorithm>
#include <numeric>

namespace Game
{
    void NavigationPathBuilder::AppendPathSegmentToTheEnd(const PathSegment &pathSegment)
    {
        mPathSegments.push_back(pathSegment);
    }

    const std::vector<PathSegment> &NavigationPathBuilder::GetPathSegments() const
    {
        return mPathSegments;
    }

    const std::vector<glm::vec3> &NavigationPathBuilder::GetRoutePoints()
    {
        const auto actualPathSegmentsPointsCount = std::accumulate(mPathSegments.cbegin(), mPathSegments.cend(), 0, [](const int32_t total, const PathSegment &segment)
                                                                   { return segment.GetTotalSegmentPointsCount() + total; });
        if (actualPathSegmentsPointsCount != mCachedRoutePoints.size())
        {
            mCachedRoutePoints.clear();
            mCachedRoutePoints.reserve(actualPathSegmentsPointsCount);
            for (const auto &segment : mPathSegments)
            {
                const auto &totalPoints = segment.GetTotalSegmentPoints();
                mCachedRoutePoints.insert(mCachedRoutePoints.end(), totalPoints.cbegin(), totalPoints.cend());
            }
        }

        return mCachedRoutePoints;
    }

    glm::vec3 NavigationPathBuilder::GetRouteFirstPoint() const
    {
        return mCachedRoutePoints.size() ? mCachedRoutePoints[0] : glm::vec3();
    }
}
