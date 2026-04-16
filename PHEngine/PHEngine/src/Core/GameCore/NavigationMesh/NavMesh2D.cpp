#include "NavMesh2D.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>
#include <cmath>
#include <queue>

namespace EngineCore::NavigationMesh {

NavMesh2D::NavMesh2D(const BoundingBox2D<glm::vec2>& levelBoundingBox, const float cellSize)
    : mLevelBoundingBox(levelBoundingBox)
    , mCellSize(cellSize)
{
    Initialize();
}

void NavMesh2D::Initialize()
{
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const auto& levelMax = mLevelBoundingBox.GetMax();

    mCellsCountX = static_cast<int32_t>((levelMax.x - levelMin.x) / mCellSize);
    mCellsCountY = static_cast<int32_t>((levelMax.y - levelMin.y) / mCellSize);

    mWalkableCells.assign(mCellsCountX * mCellsCountY, true);
}

void NavMesh2D::ResetAllCellsWalkable()
{
    std::fill(mWalkableCells.begin(), mWalkableCells.end(), true);
}

void NavMesh2D::SetCellStateByWorldPosition(const glm::vec2& worldPosition, const bool isWalkable)
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const int32_t cellX = std::min(static_cast<int32_t>((worldPosition.x - levelMin.x) / mCellSize), mCellsCountX - 1);
    const int32_t cellY = std::min(static_cast<int32_t>((worldPosition.y - levelMin.y) / mCellSize), mCellsCountY - 1);

    if (cellX >= 0 && cellX < mCellsCountX && cellY >= 0 && cellY < mCellsCountY) {
        mWalkableCells[cellX * mCellsCountY + cellY] = isWalkable;
    }
}

bool NavMesh2D::IsCellWalkableByWorldPosition(const glm::vec2& worldPosition) const
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const int32_t cellX = std::min(static_cast<int32_t>((worldPosition.x - levelMin.x) / mCellSize), mCellsCountX - 1);
    const int32_t cellY = std::min(static_cast<int32_t>((worldPosition.y - levelMin.y) / mCellSize), mCellsCountY - 1);

    if (cellX >= 0 && cellX < mCellsCountX && cellY >= 0 && cellY < mCellsCountY) {
        return mWalkableCells[cellX * mCellsCountY + cellY];
    }
    return false;
}

void NavMesh2D::FillCellStatesBetweenWorldPositions(
    const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition, const bool isWalkable)
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
        SetCellStateByWorldPosition(point, isWalkable);
    }
}

std::vector<glm::vec2>
NavMesh2D::BuildRouteBetweenPoints(const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition) const
{
    LogInfo("NavMesh2D::BuildRouteBetweenPoints: start: ", startWorldPosition, "), end: (", endWorldPosition, ")");
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const int32_t colsCount = mCellsCountX;
    const int32_t rowsCount = mCellsCountY;
    const int32_t totalCells = colsCount * rowsCount;

    auto cellIndex = [rowsCount](const int32_t x, const int32_t y) -> int32_t { return x * rowsCount + y; };

    auto worldToCell = [&](const glm::vec2& worldPos, int32_t& outX, int32_t& outY) {
        outX = std::min(static_cast<int32_t>((worldPos.x - levelMin.x) / mCellSize), colsCount - 1);
        outY = std::min(static_cast<int32_t>((worldPos.y - levelMin.y) / mCellSize), rowsCount - 1);
    };

    auto cellToWorld = [&](const int32_t cx, const int32_t cy) -> glm::vec2 {
        return {
            levelMin.x + (static_cast<float>(cx) + 0.5f) * mCellSize, levelMin.y + (static_cast<float>(cy) + 0.5f) * mCellSize};
    };

    auto isValid = [&](const int32_t cx, const int32_t cy) -> bool {
        return cx >= 0 && cx < colsCount && cy >= 0 && cy < rowsCount && mWalkableCells[cellIndex(cx, cy)];
    };

    int32_t startX, startY, endX, endY;
    worldToCell(startWorldPosition, startX, startY);
    worldToCell(endWorldPosition, endX, endY);

    if (!isValid(startX, startY) || !isValid(endX, endY)) {
        LogInfo(
            "NavMesh2D::BuildRouteBetweenPoints: start or end cell is not walkable, startCell: (",
            startX,
            ", ",
            startY,
            "), endCell: (",
            endX,
            ", ",
            endY,
            ")");
        return {};
    }

    const int32_t startIdx = cellIndex(startX, startY);
    const int32_t endIdx = cellIndex(endX, endY);

    auto heuristic = [](const int32_t ax, const int32_t ay, const int32_t bx, const int32_t by) -> float {
        return static_cast<float>(std::abs(ax - bx) + std::abs(ay - by));
    };

    struct AStarNode {
        int32_t idx;
        float fCost;
        bool operator>(const AStarNode& other) const
        {
            return fCost > other.fCost;
        }
    };

    static constexpr int32_t c_dx[] = {0, 0, 1, -1};
    static constexpr int32_t c_dy[] = {1, -1, 0, 0};

    std::vector<float> gScore(totalCells, std::numeric_limits<float>::max());
    std::vector<int32_t> cameFrom(totalCells, -1);
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;

    gScore[startIdx] = 0.0f;
    openSet.push({startIdx, heuristic(startX, startY, endX, endY)});

    while (!openSet.empty()) {
        const AStarNode current = openSet.top();
        openSet.pop();

        if (current.idx == endIdx) {
            std::vector<glm::vec2> path;
            int32_t traceIdx = endIdx;
            while (traceIdx != startIdx) {
                const int32_t tx = traceIdx / rowsCount;
                const int32_t ty = traceIdx % rowsCount;
                path.emplace_back(cellToWorld(tx, ty));
                traceIdx = cameFrom[traceIdx];
            }
            path.emplace_back(cellToWorld(startX, startY));
            std::reverse(path.begin(), path.end());
            LogInfo("NavMesh2D::BuildRouteBetweenPoints: path found with ", path.size(), " points");
            return path;
        }

        const int32_t cx = current.idx / rowsCount;
        const int32_t cy = current.idx % rowsCount;
        const float currentG = gScore[current.idx];

        if (current.fCost > currentG + heuristic(cx, cy, endX, endY) + 0.001f) {
            continue;
        }

        for (int32_t d = 0; d < 4; ++d) {
            const int32_t nx = cx + c_dx[d];
            const int32_t ny = cy + c_dy[d];
            if (!isValid(nx, ny)) {
                continue;
            }

            const int32_t neighborIdx = cellIndex(nx, ny);
            const float tentativeG = currentG + 1.0f;
            if (tentativeG < gScore[neighborIdx]) {
                gScore[neighborIdx] = tentativeG;
                cameFrom[neighborIdx] = current.idx;
                openSet.push({neighborIdx, tentativeG + heuristic(nx, ny, endX, endY)});
            }
        }
    }

    LogInfo("NavMesh2D::BuildRouteBetweenPoints: no path found");
    return {};
}

} // namespace EngineCore::NavigationMesh