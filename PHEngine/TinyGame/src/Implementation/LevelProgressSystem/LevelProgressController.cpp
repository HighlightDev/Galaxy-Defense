#include "LevelProgressController.h"

namespace Game {
void LevelProgressController::Tick(const float deltaTime)
{
    mLvlProgressBlueprint->Tick(deltaTime);
}

void LevelProgressController::UnpausableTick(const float deltaTime)
{
}

void LevelProgressController::AddLevelProgressBlueprint(const std::shared_ptr<LevelProgressBlueprint>& lvlProgressBlueprint)
{
    mLvlProgressBlueprint = lvlProgressBlueprint;
}
} // namespace Game
