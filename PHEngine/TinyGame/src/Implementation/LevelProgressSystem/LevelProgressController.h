#pragma once

#include "Core/GameCore/ITickable.h"
#include "LevelProgressBlueprint.h"

#include <memory>

namespace Game {
class LevelProgressController : public ITickable {
    std::shared_ptr<LevelProgressBlueprint> mLvlProgressBlueprint;

public:
    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

    void AddLevelProgressBlueprint(const std::shared_ptr<LevelProgressBlueprint>& lvlProgressBlueprint);
};
} // namespace Game
