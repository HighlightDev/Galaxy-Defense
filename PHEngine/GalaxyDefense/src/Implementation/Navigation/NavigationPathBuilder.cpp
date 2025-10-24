#include "NavigationPathBuilder.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/DataProviders/GameConstants.h"

namespace Game {
void NavigationPathBuilder::AddPath(const std::string& pathName, const Path& pathSegment)
{
    assert(!mPaths.count(pathName));
    mPaths[pathName] = pathSegment;
}

Path NavigationPathBuilder::GetPathByName(const std::string& name) const
{
    assert(mPaths.count(name));
    return mPaths.at(name);
}

bool NavigationPathBuilder::TryToRemovePathByName(const std::string& name)
{
    if (mPaths.count(name)) {
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

std::unordered_multimap<std::string, std::pair<std::string, Path>>& NavigationPathBuilder::GetExtendedPaths()
{
    return mExtendedPaths;
}

const std::unordered_multimap<std::string, std::pair<std::string, Path>>& NavigationPathBuilder::GetExtendedPaths() const
{
    return mExtendedPaths;
}

void NavigationPathBuilder::ExtendPath(const std::string& pathName, const int32_t eachSideExtraPathCount)
{
    assert(mPaths.count(pathName));
    auto& path = mPaths.at(pathName);
    const auto& segments = path.GetPathSegments();
    assert(segments.size());

    for (const auto& segment : segments) {
        const auto& controlPoints = segment.GetQuadraticBezierControlPoints();
        glm::vec3 normalToSegment = glm::normalize(
            glm::cross(glm::normalize(controlPoints[2] - controlPoints[0]), glm::normalize(EngineMath::AXIS_UP)));
        for (int32_t i = 0; i < eachSideExtraPathCount; ++i) {
            Path positiveNormalSideNewExtendedPath, negativeNormalSideNewExtendedPath;

            // positive normal side
            PathSegment positiveNormalSideExtraSegment;
            positiveNormalSideExtraSegment.SetSubdivisionsCount(segment.GetSubdivisionCount());
            const float offsetFactor = (static_cast<float>(i) + 1.0f) * Game::Constants::c_spaceshipSize * 1.5f;
            positiveNormalSideExtraSegment.SetControlPoints({{
                controlPoints[0] + normalToSegment * offsetFactor,
                controlPoints[1] + normalToSegment * offsetFactor,
                controlPoints[2] + normalToSegment * offsetFactor,
            }});
            positiveNormalSideNewExtendedPath.AppendPathSegmentToTheEnd(positiveNormalSideExtraSegment);

            // negative normal side
            PathSegment negativeNormalSideExtraSegment;
            negativeNormalSideExtraSegment.SetSubdivisionsCount(segment.GetSubdivisionCount());
            negativeNormalSideExtraSegment.SetControlPoints({{
                controlPoints[0] - normalToSegment * offsetFactor,
                controlPoints[1] - normalToSegment * offsetFactor,
                controlPoints[2] - normalToSegment * offsetFactor,
            }});
            negativeNormalSideNewExtendedPath.AppendPathSegmentToTheEnd(negativeNormalSideExtraSegment);

            mExtendedPaths.emplace(
                pathName, std::make_pair("positive_" + std::to_string(i) + pathName, positiveNormalSideNewExtendedPath));
            mExtendedPaths.emplace(
                pathName, std::make_pair("negative_" + std::to_string(i) + pathName, negativeNormalSideNewExtendedPath));
        }
    }
}
} // namespace Game
