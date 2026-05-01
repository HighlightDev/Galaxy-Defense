#include "LootActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game {
LootActor::LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
    , mLootCategory(eLootCategory::CRYSTAL)
{
}

eLootCategory LootActor::GetLootCategory() const
{
    return mLootCategory;
}

void LootActor::SetLootCategory(const eLootCategory lootCategory)
{
    mLootCategory = lootCategory;
}

void LootActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);
}

} // namespace Game