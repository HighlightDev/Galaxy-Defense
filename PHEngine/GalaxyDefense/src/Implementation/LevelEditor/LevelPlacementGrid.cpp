#include "LevelPlacementGrid.h"
#include "Core/CommonCore/Assertion.h"

#include <cmath>
#include <glm/vec2.hpp>

namespace Game
{
    LevelPlacementGrid::LevelPlacementGrid(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox)
        : mTowerLevelAreaBoundingBox(levelAreaBoundingBox),
          mRouteLevelAreaBoundingBox(levelAreaBoundingBox)
    {
        Initialize();
    }

    void LevelPlacementGrid::Initialize()
    {
        InitializeTowerGridData();
        InitializeRouteGridData();
    }

    void LevelPlacementGrid::InitializeTowerGridData()
    {
        const glm::vec2 levelAreaToGridCellAspectRatio = glm::vec2(static_cast<float>(mTowerLevelAreaBoundingBox.GetHalfExtent().x) * 2.0f * s_gridCellSizeForTowerInv,
                                                                   static_cast<float>(mTowerLevelAreaBoundingBox.GetHalfExtent().y) * 2.0f * s_gridCellSizeForTowerInv);
        const auto levelGridDimensionsIntPart = glm::floor(levelAreaToGridCellAspectRatio);
        mTowerGridColumnsAndRowsCount = glm::ivec2(static_cast<int32_t>(levelGridDimensionsIntPart.x), static_cast<int32_t>(levelGridDimensionsIntPart.y));
        assert(mTowerGridColumnsAndRowsCount.x >= 2 && mTowerGridColumnsAndRowsCount.y >= 2);
        mTowerLevelAreaBoundingBox = BoundingBox2D<glm::vec2>(glm::vec2(0.0f), glm::vec2(levelGridDimensionsIntPart * s_gridCellSizeForTower * 0.5f));
    }

    void LevelPlacementGrid::InitializeRouteGridData()
    {
        const glm::vec2 levelAreaToGridCellAspectRatio = glm::vec2(static_cast<float>(mRouteLevelAreaBoundingBox.GetHalfExtent().x + (s_gridCellSizeForRoute * 0.5f)) * 2.0f * s_gridCellSizeForRouteInv,
                                                                   static_cast<float>(mRouteLevelAreaBoundingBox.GetHalfExtent().y + (s_gridCellSizeForRoute * 0.5f)) * 2.0f * s_gridCellSizeForRouteInv);
        const auto levelGridDimensionsIntPart = glm::floor(levelAreaToGridCellAspectRatio);
        mRouteGridColumnsAndRowsCount = glm::ivec2(static_cast<int32_t>(levelGridDimensionsIntPart.x), static_cast<int32_t>(levelGridDimensionsIntPart.y));
        assert(mRouteGridColumnsAndRowsCount.x >= 2 && mRouteGridColumnsAndRowsCount.y >= 2);
        mRouteLevelAreaBoundingBox = BoundingBox2D<glm::vec2>(glm::vec2(0.0f), glm::vec2(levelGridDimensionsIntPart * s_gridCellSizeForRoute * 0.5f));
    }

    glm::ivec2 LevelPlacementGrid::GetTowerGridColumnsAndRowsCount() const
    {
        return mTowerGridColumnsAndRowsCount;
    }

    glm::ivec2 LevelPlacementGrid::GetRouteGridColumnsAndRowsCount() const
    {
        return mRouteGridColumnsAndRowsCount;
    }

    float LevelPlacementGrid::GetGridCellSizeForTower() const
    {
        return s_gridCellSizeForTower;
    }

    float LevelPlacementGrid::GetGridCellSizeForRoute() const
    {
        return s_gridCellSizeForRoute;
    }

    const BoundingBox2D<glm::vec2> &LevelPlacementGrid::GetTowerLevelAreaBoundingBox() const
    {
        return mTowerLevelAreaBoundingBox;
    }

    const BoundingBox2D<glm::vec2> &LevelPlacementGrid::GetRouteLevelAreaBoundingBox() const
    {
        return mRouteLevelAreaBoundingBox;
    }

    BoundingBox2D<glm::vec2> LevelPlacementGrid::GetNearestToPositionTowerCellBoundingBox(const glm::vec2 &xzPosition) const
    {
        const auto &gridColumnAndRowIndices = xzPosition * s_gridCellSizeForTowerInv;
        const auto &nearestLowBoundaryIndices = glm::floor(gridColumnAndRowIndices);
        const auto &nearestHighBoundaryIndices = glm::ceil(gridColumnAndRowIndices);
        const auto &nearestLowBoundary = nearestLowBoundaryIndices * s_gridCellSizeForTower;
        const auto &nearestHighBoundary = nearestHighBoundaryIndices * s_gridCellSizeForTower;
        const auto &boundaryExtent = ((nearestHighBoundary - nearestLowBoundary) * 0.5f);
        return BoundingBox2D<glm::vec2>(nearestLowBoundary + boundaryExtent, boundaryExtent);
    }

    glm::vec2 LevelPlacementGrid::GetNearestToPositionRouteEdgeNode(const glm::vec2 &xzPosition) const
    {
        const auto &gridColumnAndRowIndices = xzPosition * s_gridCellSizeForRouteInv;
        const auto &nearestRoundedBoundaryIndices = glm::round(gridColumnAndRowIndices);
        const auto &nearestRoundedBoundaryPosition = nearestRoundedBoundaryIndices * s_gridCellSizeForRoute + (s_gridCellSizeForRoute * 0.5f);
        return nearestRoundedBoundaryPosition;
    }
}
