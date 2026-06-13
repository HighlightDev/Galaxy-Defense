#pragma once

#include <glm/vec3.hpp>

#include <array>
#include <vector>

namespace Game {
class PathSegment {
    std::array<glm::vec3, 3> mQuadraticBezierControlPoints;

    uint32_t mSubdivisionsCount{1};

    std::vector<glm::vec3> mTotalSegmentPoints;

public:
    void SetControlPoints(const std::array<glm::vec3, 3>& controlPointsArray);

    const std::array<glm::vec3, 3>& GetQuadraticBezierControlPoints() const;

    void SetSubdivisionsCount(const uint32_t subdivisionsCount);

    uint32_t GetSubdivisionCount() const;

    const std::vector<glm::vec3>& GetTotalSegmentPoints() const;

    uint32_t GetTotalSegmentPointsCount() const;

private:
    void RecalculateTotalSegmentPoints();
};
} // namespace Game
