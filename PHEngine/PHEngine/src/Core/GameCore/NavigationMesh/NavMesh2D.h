#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <vector>

using namespace EngineCore::GUI;

namespace EngineCore::NavigationMesh {

class NavMesh2D {

    const BoundingBox2D<glm::vec2> mLevelBoundingBox;

    const float mCellSize;

    std::vector<std::vector<bool>> mWalkableCells;

public:
    explicit NavMesh2D(const BoundingBox2D<glm::vec2>& levelBoundingBox, const float cellSize);

    void SetCellStateByWorldPosition(const glm::vec2& worldPosition, const bool isWalkable);

    void FillCellStatesBetweenWorldPositions(
        const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition, const bool isWalkable);

    bool IsCellWalkableByWorldPosition(const glm::vec2& worldPosition) const;

    std::vector<glm::vec2> BuildRouteBetweenPoints(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition) const;

    const BoundingBox2D<glm::vec2>& GetLevelBoundingBox() const
    {
        return mLevelBoundingBox;
    }

    float GetCellSize() const
    {
        return mCellSize;
    }

    const std::vector<std::vector<bool>>& GetWalkableCells() const
    {
        return mWalkableCells;
    }

    void ResetAllCellsWalkable();

private:
    void Initialize();
};
} // namespace EngineCore::NavigationMesh