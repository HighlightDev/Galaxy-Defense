#include "LootController.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/LootActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/LootCategoryType.h"

using namespace EngineCore;

namespace Game {

LootController::LootController(const std::weak_ptr<EngineCore::Scene>& scene)
    : mSceneWp(scene)
    , mCombatActorsPoolHandler()
{
}

LootController::~LootController() = default;

void LootController::SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
{
    mCombatActorsPoolHandler = combatActorsPoolHandler;
    mCombatActorsPoolHandler->SetOnSpaceshipSpawnedCallback(
        [selfWp = weak_from_this()](const std::shared_ptr<SpaceshipActor>& spaceship) {
            if (const auto selfSp = selfWp.lock()) {
                selfSp->SubscribeSpaceship(spaceship);
            }
        });
}

void LootController::OnPreLevelInit()
{
}

void LootController::OnLevelInit()
{
}

void LootController::OnPostLevelInit()
{
    ext_assert(mCombatActorsPoolHandler, "CombatActorsPoolHandler is not set in LootController::OnPostLevelInit");
    mCombatActorsPoolHandler->SpawnLoot(10, eLootCategory::LOOT);
}

void LootController::PostPlayLevelFinished()
{
}

void LootController::CleanUp()
{
}

void LootController::Tick(const float deltaTimeSec)
{
}

void LootController::UnpausableTick(const float deltaTimeSec)
{
}

void LootController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

void LootController::TrySpawnLootAt(const glm::vec3& position)
{
    constexpr float kLootDropChance = 1.0f;
    if (Random::Float() < kLootDropChance) {
        if (const auto lootActor = mCombatActorsPoolHandler->GetFreeLootActor(eLootCategory::LOOT)) {
            lootActor->SpawnLoot(glm::vec3(position.x, 0.0f, position.z));
        }
    }
}

void LootController::SubscribeSpaceship(const std::shared_ptr<SpaceshipActor>& spaceship)
{
    spaceship->SetOnExplosionCallback([selfWp = weak_from_this()](const glm::vec3& position) {
        if (const auto selfSp = selfWp.lock()) {
            selfSp->TrySpawnLootAt(position);
        }
    });
}

void LootController::SubscribeToEnemySpaceships(const std::vector<std::shared_ptr<SpaceshipActor>>& spaceships)
{
    for (const auto& spaceship : spaceships) {
        SubscribeSpaceship(spaceship);
    }
}

} // namespace Game