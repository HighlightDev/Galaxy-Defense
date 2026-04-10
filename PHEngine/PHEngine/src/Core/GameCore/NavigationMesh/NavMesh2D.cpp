#include "NavMesh2D.h"

#include "Core/GameCore/LoggerExtension.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

namespace EngineCore::NavigationMesh {

struct GridCell {
    int32_t x;
    int32_t y;

    bool operator==(const GridCell& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct GridCellHash {
    size_t operator()(const GridCell& cell) const
    {
        return std::hash<int64_t>()(static_cast<int64_t>(cell.x) << 32 | static_cast<uint32_t>(cell.y));
    }
};

struct AStarNode {
    GridCell cell;
    float gCost;
    float fCost;

    bool operator>(const AStarNode& other) const
    {
        return fCost > other.fCost;
    }
};

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

    const int32_t cellsCountX = static_cast<int32_t>((levelMax.x - levelMin.x) / mCellSize);
    const int32_t cellsCountY = static_cast<int32_t>((levelMax.y - levelMin.y) / mCellSize);

    mWalkableCells.resize(cellsCountX);
    for (auto& column : mWalkableCells) {
        column.resize(cellsCountY, true);
    }
}

void NavMesh2D::ResetAllCellsWalkable()
{
    for (auto& column : mWalkableCells) {
        std::fill(column.begin(), column.end(), true);
    }
}

void NavMesh2D::SetCellStateByWorldPosition(const glm::vec2& worldPosition, const bool isWalkable)
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const int32_t cellX = static_cast<int32_t>((worldPosition.x - levelMin.x) / mCellSize);
    const int32_t cellY = static_cast<int32_t>((worldPosition.y - levelMin.y) / mCellSize);

    if (cellX >= 0 && cellX < mWalkableCells.size() && cellY >= 0 && cellY < mWalkableCells[cellX].size()) {
        EngineCore::LogInfo(
            "NaMesh2D: Setting cell state at (", cellX, ", ", cellY, ") to ", (isWalkable ? "walkable" : "non-walkable"));
        mWalkableCells[cellX][cellY] = isWalkable;
    }
}

bool NavMesh2D::IsCellWalkableByWorldPosition(const glm::vec2& worldPosition) const
{
    const auto& levelMin = mLevelBoundingBox.GetMin();

    const int32_t cellX = static_cast<int32_t>((worldPosition.x - levelMin.x) / mCellSize);
    const int32_t cellY = static_cast<int32_t>((worldPosition.y - levelMin.y) / mCellSize);

    if (cellX >= 0 && cellX < mWalkableCells.size() && cellY >= 0 && cellY < mWalkableCells[cellX].size()) {
        return mWalkableCells[cellX][cellY];
    }
    return false;
}

void NavMesh2D::FillCellStatesBetweenWorldPositions(
    const glm::vec2& startWorldPosition, const glm::vec2& endWorldPosition, const bool isWalkable)
{
    const float c_invCellSize = 1.0f / mCellSize;

    const int32_t cellX1 = static_cast<int32_t>((startWorldPosition.x - mLevelBoundingBox.GetMin().x) * c_invCellSize);
    const int32_t cellY1 = static_cast<int32_t>((startWorldPosition.y - mLevelBoundingBox.GetMin().y) * c_invCellSize);
    const int32_t cellX2 = static_cast<int32_t>((endWorldPosition.x - mLevelBoundingBox.GetMin().x) * c_invCellSize);
    const int32_t cellY2 = static_cast<int32_t>((endWorldPosition.y - mLevelBoundingBox.GetMin().y) * c_invCellSize);

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
    const auto& levelMin = mLevelBoundingBox.GetMin();
    const int32_t colsCount = static_cast<int32_t>(mWalkableCells.size());
    const int32_t rowsCount = colsCount > 0 ? static_cast<int32_t>(mWalkableCells[0].size()) : 0;

    auto worldToCell = [&](const glm::vec2& worldPos) -> GridCell {
        return {
            static_cast<int32_t>((worldPos.x - levelMin.x) / mCellSize),
            static_cast<int32_t>((worldPos.y - levelMin.y) / mCellSize)};
    };

    auto cellToWorld = [&](const GridCell& cell) -> glm::vec2 {
        return {
            levelMin.x + (static_cast<float>(cell.x) + 0.5f) * mCellSize,
            levelMin.y + (static_cast<float>(cell.y) + 0.5f) * mCellSize};
    };

    auto isValid = [&](const GridCell& cell) -> bool {
        return cell.x >= 0 && cell.x < colsCount && cell.y >= 0 && cell.y < rowsCount && mWalkableCells[cell.x][cell.y];
    };

    const GridCell startCell = worldToCell(startWorldPosition);
    const GridCell endCell = worldToCell(endWorldPosition);

    if (!isValid(startCell) || !isValid(endCell)) {
        return {};
    }

    auto heuristic = [](const GridCell& a, const GridCell& b) -> float {
        const float dx = static_cast<float>(std::abs(a.x - b.x));
        const float dy = static_cast<float>(std::abs(a.y - b.y));
        return dx + dy;
    };

    static constexpr std::array<std::pair<int32_t, int32_t>, 4> c_directions = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::unordered_map<GridCell, GridCell, GridCellHash> cameFrom;
    std::unordered_map<GridCell, float, GridCellHash> gScore;

    gScore[startCell] = 0.0f;
    openSet.push({startCell, 0.0f, heuristic(startCell, endCell)});

    while (!openSet.empty()) {
        const AStarNode current = openSet.top();
        openSet.pop();

        if (current.cell == endCell) {
            std::vector<glm::vec2> path;
            GridCell traceCell = endCell;
            while (!(traceCell == startCell)) {
                path.emplace_back(cellToWorld(traceCell));
                traceCell = cameFrom.at(traceCell);
            }
            path.emplace_back(cellToWorld(startCell));
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (current.gCost > gScore[current.cell]) {
            continue;
        }

        for (const auto& [dx, dy] : c_directions) {
            const GridCell neighbor = {current.cell.x + dx, current.cell.y + dy};
            if (!isValid(neighbor)) {
                continue;
            }

            const float tentativeG = current.gCost + 1.0f;
            const auto it = gScore.find(neighbor);
            if (it == gScore.end() || tentativeG < it->second) {
                gScore[neighbor] = tentativeG;
                cameFrom[neighbor] = current.cell;
                openSet.push({neighbor, tentativeG, tentativeG + heuristic(neighbor, endCell)});
            }
        }
    }

    return {};
}

} // namespace EngineCore::NavigationMesh