#include "LootActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Components/MovementComponents/LootDropMovementComponent.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game {
LootActor::LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
    , mLootCategory(eLootCategory::LOOT)
    , mLootState(eLootState::NA)
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

void LootActor::SetLootState(const eLootState lootState)
{
    if (mLootState != lootState) {
        mLootState = lootState;
        SetIsEnabled(mLootState == eLootState::ACTIVE);

        const auto particleComponents = GetComponentsByType<ParticleSystemBaseComponent>();
        for (const auto& particleComponent : particleComponents) {
            if (mLootState != eLootState::ACTIVE) {
                particleComponent->ResetParticles();
                const auto movementComponents = std::dynamic_pointer_cast<LootDropMovementComponent>(GetMovementComponent());
                if (movementComponents) {
                    movementComponents->Reset();
                }
            } else {
                particleComponent->EmitParticles();
            }
        }
    }
}

eLootState LootActor::GetLootState() const
{
    return mLootState;
}

void LootActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);
}

void LootActor::CollectLoot()
{
    LogInfo(
        "LootActor::CollectLoot: name: ",
        GetName(),
        " state: ",
        static_cast<int>(mLootState),
        " category: ",
        static_cast<int>(mLootCategory));
    SetLootState(eLootState::IDLE);
    if (mLootCategory == eLootCategory::LOOT) {
        PlayerDataProvider::GetInstance()->SetCrystalsCount(PlayerDataProvider::GetInstance()->GetCrystalsCount() + 1);
    }
}

void LootActor::SpawnLoot(const glm::vec3& position)
{
    LogInfo("LootActor::SpawnLoot: name: ", GetName(), " position: ", position.x, ", ", position.y, ", ", position.z);
    GetRootComponent()->SetTranslation(position);
    SetLootState(eLootState::ACTIVE);
}

} // namespace Game