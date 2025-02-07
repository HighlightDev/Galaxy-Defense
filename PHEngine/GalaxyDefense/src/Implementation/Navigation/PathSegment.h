#pragma once

#include <glm/vec3.hpp>

#include <array>
#include <vector>

namespace Game {
class PathSegment {
    std::array<glm::vec3, 3> mQuadraticBezierControlPoints;

    size_t mSubdivisionsCount{1};

    std::vector<glm::vec3> mTotalSegmentPoints;

public:
    void SetControlPoints(const std::array<glm::vec3, 3>& controlPointsArray);

    const std::array<glm::vec3, 3>& GetQuadraticBezierControlPoints() const;

    void SetSubdivisionsCount(const size_t subdivisionsCount);

    size_t GetSubdivisionCount() const;

    const std::vector<glm::vec3>& GetTotalSegmentPoints() const;

    size_t GetTotalSegmentPointsCount() const;

private:
    void RecalculateTotalSegmentPoints();
};
} // namespace Game
