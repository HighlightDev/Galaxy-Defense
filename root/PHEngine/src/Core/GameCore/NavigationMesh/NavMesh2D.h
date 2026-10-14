#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <vector>

using namespace EngineCore::GUI;

namespace EngineCore::NavigationMesh {

class NavMesh2D {

    const BoundingBox2D<glm::vec2> mLevelBoundingBox;

    const float mCellSize;

    int32_t mCellsCountX{0};

    int32_t mCellsCountY{0};

    std::vector<bool> mWalkableCells;

public:
    explicit NavMesh2D(const BoundingBox2D<glm::vec2>& levelBoundingBox, const float cellSize);

    void SetCellsStateByWorldPosition(const glm::vec2& worldPosition, const glm::vec2& size, const bool isWalkable);

    void FillCellStatesBetweenWorldPositions(
        const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition, const glm::vec2& size, const bool isWalkable);

    bool IsCellWalkableByWorldPosition(const glm::vec2& worldPosition) const;

    bool IsSegmentWalkable(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition) const;

    std::vector<glm::vec2> BuildRouteBetweenPoints(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition) const;

    const BoundingBox2D<glm::vec2>& GetLevelBoundingBox() const
    {
        return mLevelBoundingBox;
    }

    float GetCellSize() const
    {
        return mCellSize;
    }

    int32_t GetCellsCountX() const
    {
        return mCellsCountX;
    }

    int32_t GetCellsCountY() const
    {
        return mCellsCountY;
    }

    bool IsCellWalkable(const int32_t x, const int32_t y) const
    {
        return mWalkableCells[x * mCellsCountY + y];
    }

    void ResetAllCellsWalkable();

private:
    void Initialize();
};
} // namespace EngineCore::NavigationMesh