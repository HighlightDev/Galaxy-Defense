#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

using namespace EngineCore::GUI;

namespace Game
{
    class LevelPlacementGrid
    {
        static constexpr float s_gridCellSizeForTower = 20.0f;
        static constexpr float s_gridCellSizeForRoute = s_gridCellSizeForTower * 0.5f;
        static constexpr float s_gridCellSizeForTowerInv = 1.0f / s_gridCellSizeForTower;
        static constexpr float s_gridCellSizeForRouteInv = 1.0f / s_gridCellSizeForRoute;

        BoundingBox2D<glm::vec2> mTowerLevelAreaBoundingBox;

        BoundingBox2D<glm::vec2> mRouteLevelAreaBoundingBox;

        glm::ivec2 mTowerGridColumnsAndRowsCount;

        glm::ivec2 mRouteGridColumnsAndRowsCount;

    public:
        explicit LevelPlacementGrid(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox);

        glm::ivec2 GetTowerGridColumnsAndRowsCount() const;

        glm::ivec2 GetRouteGridColumnsAndRowsCount() const;

        const BoundingBox2D<glm::vec2> &GetTowerLevelAreaBoundingBox() const;

        const BoundingBox2D<glm::vec2> &GetRouteLevelAreaBoundingBox() const;

        float GetGridCellSizeForTower() const;

        float GetGridCellSizeForRoute() const;

        BoundingBox2D<glm::vec2> GetNearestToPositionTowerCellBoundingBox(const glm::vec2 &xzPosition) const;

    private:
        void Initialize();

        void InitializeTowerGridData();

        void InitializeRouteGridData();
    };
} // namespace Game
