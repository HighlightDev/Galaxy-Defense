#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <glm/vec2.hpp>
#include <stdint.h>

#include <cstdint>

using namespace EngineCore::GUI;

namespace Game {
class LevelDataProvider {
    int32_t mCurrentStageSurvivedEnemySpaceshipsCount{0};

    BoundingBox2D<glm::vec2> mEditorLevelAreaBoundingBox;

private:
    LevelDataProvider() = default;

public:
    static LevelDataProvider* GetInstance();

    int32_t GetCurrentStageSurvivedEnemySpaceshipsCount() const;

    void SetCurrentStageSurvivedEnemySpaceshipsCount(const int32_t count);

    const BoundingBox2D<glm::vec2>& GetEditorLevelAreaBoundingBox() const;

    void SetEditorLevelAreaBoundingBox(
        const BoundingBox2D<glm::vec2>& boundingBox,
        const bool notifyGameThreadSubscribers,
        const bool notifyLuaThreadSubscribers);
};
} // namespace Game
