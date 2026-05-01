#pragma once

#include "Core/GameCore/Actor.h"
#include "Implementation/LootCategoryType.h"

using namespace EngineCore;

namespace EngineCore {
class SceneComponent;
}

namespace Game {

class LootActor : public Actor {

    eLootCategory mLootCategory;

public:
    LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec) override;

    eLootCategory GetLootCategory() const;

    void SetLootCategory(const eLootCategory lootCategory);
};
} // namespace Game