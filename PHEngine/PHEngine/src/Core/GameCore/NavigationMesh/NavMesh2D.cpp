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
            if (cellX >= 0 && cellX < mCellsCountX && cellY >= 0 && cellY < mCellsCountY) {
                mWalkableCells[cellX * mCellsCountY + cellY] = isWalkable;
            }
        }
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

    auto inBounds
        = [&](const int32_t cx, const int32_t cy) -> bool { return cx >= 0 && cx < colsCount && cy >= 0 && cy < rowsCount; };

    int32_t startX, startY, endX, endY;
    worldToCell(startWorldPosition, startX, startY);
    worldToCell(endWorldPosition, endX, endY);

    if (!inBounds(startX, startY) || !inBounds(endX, endY)) {
        LogInfo(
            "NavMesh2D::BuildRouteBetweenPoints: start or end cell is out of bounds, startCell: (",
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

    // Start/end cells are allowed to be non-walkable: the start is where the ship already is,
    // and the end is the goal we want to reach (a barrier ray can clip the destination cell).
    // They are valid endpoints but cannot be used as pass-through cells.
    auto isValid = [&](const int32_t cx, const int32_t cy) -> bool {
        if (!inBounds(cx, cy)) {
            return false;
        }
        if ((cx == startX && cy == startY) || (cx == endX && cy == endY)) {
            return true;
        }
        return mWalkableCells[cellIndex(cx, cy)];
    };

    const int32_t startIdx = cellIndex(startX, startY);
    const int32_t endIdx = cellIndex(endX, endY);

    auto heuristic = [](const int32_t ax, const int32_t ay, const int32_t bx, const int32_t by) -> float {
        const float dx = static_cast<float>(std::abs(ax - bx));
        const float dy = static_cast<float>(std::abs(ay - by));
        // Octile distance: accounts for diagonal moves costing sqrt(2)
        return std::max(dx, dy) + (1.41421356f - 1.0f) * std::min(dx, dy);
    };

    struct AStarNode {
        int32_t idx;
        float fCost;
        bool operator>(const AStarNode& other) const
        {
            return fCost > other.fCost;
        }
    };

    // Cardinals first (indices 0-3), then diagonals (indices 4-7)
    static constexpr int32_t c_dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
    static constexpr int32_t c_dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
    static constexpr float c_moveCost[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.41421356f, 1.41421356f, 1.41421356f, 1.41421356f};

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

        for (int32_t d = 0; d < 8; ++d) {
            const int32_t nx = cx + c_dx[d];
            const int32_t ny = cy + c_dy[d];
            if (!isValid(nx, ny)) {
                continue;
            }
            // Prevent corner cutting: both cardinal neighbors must be walkable for diagonal moves
            if (d >= 4 && (!isValid(cx + c_dx[d], cy) || !isValid(cx, cy + c_dy[d]))) {
                continue;
            }

            const int32_t neighborIdx = cellIndex(nx, ny);
            const float tentativeG = currentG + c_moveCost[d];
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