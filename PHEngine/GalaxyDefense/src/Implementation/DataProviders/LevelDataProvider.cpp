#include "LevelDataProvider.h"

#include "Implementation/Events/LevelAreaBBChangedEvent.h"

namespace Game {
LevelDataProvider* LevelDataProvider::GetInstance()
{
    static LevelDataProvider levelDataProvider;
    return &levelDataProvider;
}

int32_t LevelDataProvider::GetCurrentStageSurvivedEnemySpaceshipsCount() const
{
    return mCurrentStageSurvivedEnemySpaceshipsCount;
}

void LevelDataProvider::SetCurrentStageSurvivedEnemySpaceshipsCount(const int32_t count)
{
    mCurrentStageSurvivedEnemySpaceshipsCount = count;
}

const BoundingBox2D<glm::vec2>& LevelDataProvider::GetEditorLevelAreaBoundingBox() const
{
    return mEditorLevelAreaBoundingBox;
}

void LevelDataProvider::SetEditorLevelAreaBoundingBox(
    const BoundingBox2D<glm::vec2>& boundingBox, const bool notifyGameThreadSubscribers, const bool notifyLuaThreadSubscribers)
{
    mEditorLevelAreaBoundingBox = boundingBox;
    if (notifyGameThreadSubscribers) {
        Event::LevelAreaBBChangedGameThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::PRE_EXECUTION, mEditorLevelAreaBoundingBox);
    }
    if (notifyLuaThreadSubscribers) {
        Event::LevelAreaBBChangedLuaThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::PRE_EXECUTION, mEditorLevelAreaBoundingBox);
    }
}
} // namespace Game
