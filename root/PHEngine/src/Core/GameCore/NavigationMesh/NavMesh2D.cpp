#include "NavMesh2D.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>
#include <cmath>
#include <queue>

using namespace PathfindingLib;

namespace EngineCore::NavigationMesh {

NavMesh2D::NavMesh2D(const BoundingBox2D<glm::vec2>& levelBoundingBox, const float cellSize)
    : mLevelBoundingBox(levelBoundingBox)
    , mCellSize(cellSize)
    , mPathfindingGrid(1, 1) // As grid doesn't have default constructor - create dummy instance first
{
    Initialize();
}

void NavMesh2D::Initialize()
{
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const auto& levelMax = mLevelBoundingBox.GetMax();

    mCellsCountX = static_cast<int32_t>((levelMax.x - levelMin.x) / mCellSize);
    mCellsCountY = static_cast<int32_t>((levelMax.y - levelMin.y) / mCellSize);

    mPathfindingGrid = Pathfinding_Grid<int32_t>(mCellsCountX, mCellsCountY);
}

void NavMesh2D::ResetAllCellsWalkable()
{
    mPathfindingGrid = Pathfinding_Grid<int32_t>(mCellsCountX, mCellsCountY);
}

const BoundingBox2D<glm::vec2>& NavMesh2D::GetLevelBoundingBox() const
{
    return mLevelBoundingBox;
}

float NavMesh2D::GetCellSize() const
{
    return mCellSize;
}

int32_t NavMesh2D::GetCellsCountX() const
{
    return mCellsCountX;
}

int32_t NavMesh2D::GetCellsCountY() const
{
    return mCellsCountY;
}

bool NavMesh2D::IsCellWalkable(const int32_t x, const int32_t y) const
{
    if (mPathfindingGrid.isWithinBounds(x, y)) {
        return mPathfindingGrid.getNode(x, y).isWalkable;
    }
    return false;
}

void NavMesh2D::SetCellsStateByWorldPosition(const glm::vec2& worldPosition, const glm::vec2& size, const bool isWalkable)
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const auto& maxCornerPos = worldPosition + (size * 0.5f);
    const auto& minCornerPos = worldPosition - (size * 0.5f);
    const int32_t occupiedCellsX = static_cast<int32_t>(std::ceil(size.x / mCellSize));
    const int32_t occupiedCellsY = static_cast<int32_t>(std::ceil(size.y / mCellSize));

    for (int32_t x = 0; x < occupiedCellsX; ++x) {
        for (int32_t y = 0; y < occupiedCellsY; ++y) {
            const auto& cellCenter = glm::vec2(
                minCornerPos.x + mCellSize * (0.5f + static_cast<float>(x)),
                minCornerPos.y + mCellSize * (0.5f + static_cast<float>(y)));
            const int32_t cellX = std::min(static_cast<int32_t>((cellCenter.x - levelMin.x) / mCellSize), mCellsCountX - 1);
            const int32_t cellY = std::min(static_cast<int32_t>((cellCenter.y - levelMin.y) / mCellSize), mCellsCountY - 1);
            if (isWalkable) {
                mPathfindingGrid.setWalkable(cellX, cellY);
            } else {
                mPathfindingGrid.setObstacle(cellX, cellY);
            }
        }
    }
}

bool NavMesh2D::IsCellWalkableByWorldPosition(const glm::vec2& worldPosition) const
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const int32_t cellX = std::min(static_cast<int32_t>((worldPosition.x - levelMin.x) / mCellSize), mCellsCountX - 1);
    const int32_t cellY = std::min(static_cast<int32_t>((worldPosition.y - levelMin.y) / mCellSize), mCellsCountY - 1);
    if (mPathfindingGrid.isWithinBounds(cellX, cellY)) {
        return mPathfindingGrid.getNode(cellX, cellY).isWalkable;
    }
    return false;
}

bool NavMesh2D::IsSegmentWalkable(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition) const
{
    const float distance = glm::distance(startWorldPosition, endWorldPosition);
    if (distance <= 0.0f) {
        return IsCellWalkableByWorldPosition(startWorldPosition);
    }

    // Sample at half-cell stride to ensure every cell crossed by the segment is checked.
    const float stepLength = mCellSize * 0.5f;
    const int32_t steps = static_cast<int32_t>(std::ceil(distance / stepLength));
    const glm::vec2 direction = (endWorldPosition - startWorldPosition) / distance;
    for (int32_t i = 0; i <= steps; ++i) {
        const float t = std::min(static_cast<float>(i) * stepLength, distance);
        const glm::vec2 point = startWorldPosition + direction * t;
        if (!IsCellWalkableByWorldPosition(point)) {
            return false;
        }
    }
    return true;
}

void NavMesh2D::FillCellStatesBetweenWorldPositions(
    const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition, const glm::vec2& size, const bool isWalkable)
{
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const auto& levelMax = mLevelBoundingBox.GetMax();

    if (!EngineMath::TestPointInAABB(levelMin, levelMax, startWorldPosition)
        || !EngineMath::TestPointInAABB(levelMin, levelMax, endWorldPosition)) {
        LogInfo(
            "NavMesh2D::FillCellStatesBetweenWorldPositions: start or end world position is out of level bounds, start: (",
            startWorldPosition,
            "), end: (",
            endWorldPosition,
            ")");
        return;
    }

    const float c_invCellSize = 1.0f / mCellSize;

    const float distance = glm::distance(startWorldPosition, endWorldPosition);
    const glm::vec2 direction = (endWorldPosition - startWorldPosition) / distance;
    const int32_t steps = static_cast<int32_t>(distance * c_invCellSize);
    for (int32_t i = 0; i <= steps; ++i) {
        const glm::vec2 point = startWorldPosition + direction * (i * mCellSize);
        SetCellsStateByWorldPosition(point, size, isWalkable);
    }
}

std::vector<glm::vec2> NavMesh2D::BuildRouteBetweenPoints(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition)
{
    LogInfo("NavMesh2D::BuildRouteBetweenPoints: start: ", startWorldPosition, "), end: (", endWorldPosition, ")");
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const int32_t colsCount = mCellsCountX;
    const int32_t rowsCount = mCellsCountY;

    auto worldToCell = [&](const glm::vec2& worldPos, int32_t& outX, int32_t& outY) {
        outX = std::min(static_cast<int32_t>((worldPos.x - levelMin.x) / mCellSize), colsCount - 1);
        outY = std::min(static_cast<int32_t>((worldPos.y - levelMin.y) / mCellSize), rowsCount - 1);
    };

    auto cellToWorld = [&](const int32_t cx, const int32_t cy) -> glm::vec2 {
        return {
            levelMin.x + (static_cast<float>(cx) + 0.5f) * mCellSize, levelMin.y + (static_cast<float>(cy) + 0.5f) * mCellSize};
    };

    int32_t startX, startY, endX, endY;
    worldToCell(startWorldPosition, startX, startY);
    worldToCell(endWorldPosition, endX, endY);

    auto resultPathInTuples = findPathAStar(mPathfindingGrid, std::make_tuple(startX, startY), std::make_tuple(endX, endY));
    if (resultPathInTuples.empty()) {
        LogInfo("NavMesh2D::BuildRouteBetweenPoints: no path found");
        return {};
    }

    std::vector<glm::vec2> resultPath;
    resultPath.reserve(resultPathInTuples.size());
    std::transform(
        resultPathInTuples.cbegin(),
        resultPathInTuples.cend(),
        std::back_inserter(resultPath),
        [&cellToWorld](const auto& coordsTuple) {
            return glm::vec2(cellToWorld(std::get<0>(coordsTuple), std::get<1>(coordsTuple)));
        });
    return resultPath;
}

} // namespace EngineCore::NavigationMesh