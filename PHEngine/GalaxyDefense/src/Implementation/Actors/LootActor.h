#pragma once

#include "Core/GameCore/Actor.h"
#include "Implementation/LootCategoryType.h"

using namespace EngineCore;

namespace EngineCore {
class SceneComponent;
}

namespace Game {

enum class eLootState { NA, IDLE, ACTIVE };

class LootActor : public Actor {

    eLootCategory mLootCategory;

    eLootState mLootState;

public:
    LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec) override;

    eLootCategory GetLootCategory() const;

    void SetLootCategory(const eLootCategory lootCategory);

    void SetLootState(const eLootState lootState);

    eLootState GetLootState() const;

    virtual void CollectLoot();

    virtual void SpawnLoot(const glm::vec3& position);
};
} // namespace Game