#include "LevelPlacementGrid.h"

#include "Core/CommonCore/Assertion.h"

#include <glm/vec2.hpp>

#include <cmath>

namespace Game {
LevelPlacementGrid::LevelPlacementGrid(const BoundingBox2D<glm::vec2>& levelAreaBoundingBox)
    : mTowerLevelAreaBoundingBox(levelAreaBoundingBox)
    , mRouteLevelAreaBoundingBox(levelAreaBoundingBox)
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
    const glm::vec2 levelAreaToGridCellAspectRatio
        = mTowerLevelAreaBoundingBox.GetHalfExtent() * 2.0f * s_gridCellSizeForTowerInv;
    auto levelGridDimensionsIntPart = glm::floor(levelAreaToGridCellAspectRatio);
    mTowerGridColumnsAndRowsCount
        = glm::ivec2(static_cast<int32_t>(levelGridDimensionsIntPart.x), static_cast<int32_t>(levelGridDimensionsIntPart.y));
    // if (mTowerGridColumnsAndRowsCount.x % 2 != 0) {
    //     mTowerGridColumnsAndRowsCount.x += 1;
    //     levelGridDimensionsIntPart.x += s_gridCellSizeForTower;
    // }

    // if (mTowerGridColumnsAndRowsCount.y % 2 != 0) {
    //     mTowerGridColumnsAndRowsCount.y += 1;
    //     levelGridDimensionsIntPart.y += s_gridCellSizeForTower;
    // }
    assert(mTowerGridColumnsAndRowsCount.x >= 2 && mTowerGridColumnsAndRowsCount.y >= 2);
    mTowerLevelAreaBoundingBox
        = BoundingBox2D<glm::vec2>(glm::vec2(0.0f), levelGridDimensionsIntPart * s_gridCellSizeForTower * 0.5f);
}

void LevelPlacementGrid::InitializeRouteGridData()
{
    const glm::vec2 levelAreaToGridCellAspectRatio = glm::vec2(
        static_cast<float>(mRouteLevelAreaBoundingBox.GetHalfExtent().x + (s_gridCellSizeForRoute * 0.5f)) * 2.0f
            * s_gridCellSizeForRouteInv,
        static_cast<float>(mRouteLevelAreaBoundingBox.GetHalfExtent().y + (s_gridCellSizeForRoute * 0.5f)) * 2.0f
            * s_gridCellSizeForRouteInv);
    const auto levelGridDimensionsIntPart = glm::floor(levelAreaToGridCellAspectRatio);
    mRouteGridColumnsAndRowsCount
        = glm::ivec2(static_cast<int32_t>(levelGridDimensionsIntPart.x), static_cast<int32_t>(levelGridDimensionsIntPart.y));
    assert(mRouteGridColumnsAndRowsCount.x >= 2 && mRouteGridColumnsAndRowsCount.y >= 2);
    mRouteLevelAreaBoundingBox
        = BoundingBox2D<glm::vec2>(glm::vec2(0.0f), levelGridDimensionsIntPart * s_gridCellSizeForRoute * 0.5f);
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

const BoundingBox2D<glm::vec2>& LevelPlacementGrid::GetTowerLevelAreaBoundingBox() const
{
    return mTowerLevelAreaBoundingBox;
}

const BoundingBox2D<glm::vec2>& LevelPlacementGrid::GetRouteLevelAreaBoundingBox() const
{
    return mRouteLevelAreaBoundingBox;
}

BoundingBox2D<glm::vec2> LevelPlacementGrid::GetNearestToPositionTowerCellBoundingBox(const glm::vec2& xzPosition) const
{
    const auto& gridColumnAndRowIndices = xzPosition * s_gridCellSizeForTowerInv;
    const auto& nearestLowBoundaryIndices = glm::floor(gridColumnAndRowIndices);
    const auto& nearestHighBoundaryIndices = glm::ceil(gridColumnAndRowIndices);
    const auto& nearestLowBoundary = nearestLowBoundaryIndices * s_gridCellSizeForTower;
    const auto& nearestHighBoundary = nearestHighBoundaryIndices * s_gridCellSizeForTower;
    const auto& boundaryExtent = ((nearestHighBoundary - nearestLowBoundary) * 0.5f);
    return BoundingBox2D<glm::vec2>(nearestLowBoundary + boundaryExtent, boundaryExtent);
}

glm::vec2 LevelPlacementGrid::GetNearestToPositionRouteEdgeNode(const glm::vec2& xzPosition) const
{
    const auto& clampedPosition
        = glm::clamp(xzPosition, mRouteLevelAreaBoundingBox.GetMin(), mRouteLevelAreaBoundingBox.GetMax());
    const auto& normalizedPosition = (clampedPosition + mRouteLevelAreaBoundingBox.GetHalfExtent()) * 0.5f;
    const auto& gridColumnAndRowIndices = normalizedPosition * (s_gridCellSizeForRouteInv * 2.0f);
    const auto& nearestNormRoundedBoundaryIndices = glm::round(gridColumnAndRowIndices);
    const auto& nearestNormRoundedBoundaryPosition = nearestNormRoundedBoundaryIndices * (s_gridCellSizeForRoute * 0.5f);
    const auto& restoredPosition = (nearestNormRoundedBoundaryPosition * 2.0f) - mRouteLevelAreaBoundingBox.GetHalfExtent();
    return restoredPosition;
}

void LevelPlacementGrid::UpdateLevelAreaBoundingBox(const BoundingBox2D<glm::vec2>& newLvlAreaBoundingBox)
{
    mTowerLevelAreaBoundingBox = newLvlAreaBoundingBox;
    mRouteLevelAreaBoundingBox = newLvlAreaBoundingBox;
    Initialize();
}
} // namespace Game
