#include "LevelPlacementGrid.h"
#include "Core/CommonCore/Assertion.h"

#include <cmath>
#include <glm/vec2.hpp>

namespace Game
{
    LevelPlacementGrid::LevelPlacementGrid(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox)
        : mLevelAreaBoundingBox(levelAreaBoundingBox)
    {
        Initialize();
    }

    void LevelPlacementGrid::Initialize()
    {
        const glm::vec2 levelAreaToGridCellAspectRatio = glm::vec2(static_cast<float>(mLevelAreaBoundingBox.GetHalfExtent().x) * 2.0f * s_gridCellSizeForTowerInv,
                                                                   static_cast<float>(mLevelAreaBoundingBox.GetHalfExtent().y) * 2.0f * s_gridCellSizeForTowerInv);
        const auto levelGridDimensionsIntPart = glm::floor(levelAreaToGridCellAspectRatio);
        mTowerGridColumnsAndRowsCount = glm::ivec2(static_cast<int32_t>(levelGridDimensionsIntPart.x), static_cast<int32_t>(levelGridDimensionsIntPart.y));
        assert(mTowerGridColumnsAndRowsCount.x >= 2 && mTowerGridColumnsAndRowsCount.y >= 2);
        mLevelAreaBoundingBox = BoundingBox2D<glm::vec2>(glm::vec2(0.0f), glm::vec2(levelGridDimensionsIntPart * s_gridCellSizeForTower * 0.5f));
    }

    glm::ivec2 LevelPlacementGrid::GetTowerGridColumnsAndRowsCount() const
    {
        return mTowerGridColumnsAndRowsCount;
    }

    float LevelPlacementGrid::GetGridCellSizeForTower() const
    {
        return s_gridCellSizeForTower;
    }

    float LevelPlacementGrid::GetGridCellSizeForRoute() const
    {
        return s_gridCellSizeForRoute;
    }

     const BoundingBox2D<glm::vec2>& LevelPlacementGrid::GetLevelAreaBoundingBox() const
     {
        return mLevelAreaBoundingBox;
     }

}
