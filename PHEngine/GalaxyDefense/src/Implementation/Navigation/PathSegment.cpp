#include "PathSegment.h"

#include "Core/UtilityCore/EngineMath.h"

namespace Game {
const std::array<glm::vec3, 3>& PathSegment::GetQuadraticBezierControlPoints() const
{
    return mQuadraticBezierControlPoints;
}

void PathSegment::SetControlPoints(const std::array<glm::vec3, 3>& controlPointsArray)
{
    mQuadraticBezierControlPoints = controlPointsArray;
    RecalculateTotalSegmentPoints();
}

void PathSegment::SetSubdivisionsCount(const size_t subdivisionsCount)
{
    if (mSubdivisionsCount != subdivisionsCount) {
        mSubdivisionsCount = subdivisionsCount;
        RecalculateTotalSegmentPoints();
    }
}

size_t PathSegment::GetSubdivisionCount() const
{
    return mSubdivisionsCount;
}

const std::vector<glm::vec3>& PathSegment::GetTotalSegmentPoints() const
{
    return mTotalSegmentPoints;
}

size_t PathSegment::GetTotalSegmentPointsCount() const
{
    return mTotalSegmentPoints.size();
}

void PathSegment::RecalculateTotalSegmentPoints()
{
    const size_t curvePathPointsCount
        = mSubdivisionsCount <= static_cast<size_t>(1) ? static_cast<size_t>(2) : static_cast<size_t>(mSubdivisionsCount + 1);
    mTotalSegmentPoints.clear();
    mTotalSegmentPoints.reserve(curvePathPointsCount);
    mTotalSegmentPoints.emplace_back(mQuadraticBezierControlPoints.at(0));
    const float bezier_t_step = (1.0f / static_cast<float>(curvePathPointsCount));
    float bezier_t = 0.0f;
    for (size_t i = 1; i < (curvePathPointsCount); ++i) {
        bezier_t = static_cast<float>(i) * bezier_t_step;
        mTotalSegmentPoints.emplace_back(EngineMath::QuadraticBezier(
            mQuadraticBezierControlPoints.at(0),
            mQuadraticBezierControlPoints.at(1),
            mQuadraticBezierControlPoints.at(2),
            bezier_t));
    }
    mTotalSegmentPoints.emplace_back(mQuadraticBezierControlPoints.at(2));
}
} // namespace Game
