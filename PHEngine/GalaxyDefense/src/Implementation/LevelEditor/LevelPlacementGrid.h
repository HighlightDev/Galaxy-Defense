#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

using namespace EngineCore::GUI;

namespace Game
{
    class LevelPlacementGrid
    {
        static constexpr float s_gridCellSizeForTower = 20.0f;
        static constexpr float s_gridCellSizeForRoute = 5.0f;
        static constexpr float s_gridCellSizeForTowerInv = 1.0f / s_gridCellSizeForTower;
        static constexpr float s_gridCellSizeForRouteInv = 1.0f / s_gridCellSizeForRoute;

        BoundingBox2D<glm::vec2> mLevelAreaBoundingBox;

        glm::ivec2 mTowerGridColumnsAndRowsCount;

    public:
        explicit LevelPlacementGrid(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox);

        glm::ivec2 GetTowerGridColumnsAndRowsCount() const;

        const BoundingBox2D<glm::vec2> &GetLevelAreaBoundingBox() const;

        float GetGridCellSizeForTower() const;

        float GetGridCellSizeForRoute() const;

        BoundingBox2D<glm::vec2> GetNearestToPositionTowerCellBoundingBox(const glm::vec2 &xzPosition) const;

    private:
        void Initialize();
    };
} // namespace Game
