#include "LootController.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Scene.h"
#include "Game/Actors/LootActor.h"
#include "Game/Actors/SpaceshipActor.h"
#include "Game/Constants/GameConstants.h"
#include "Game/DropLootType.h"
#include "Game/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Game/LootSystem/PandoraBox.h"

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
    mCombatActorsPoolHandler->SpawnLoot(10, eDropLootType::CRYSTALS);
}

void LootController::PostPlayLevelFinished()
{
}

void LootController::CleanUp()
{
}

void LootController::Tick(const float deltaTimeSec, const float playSpeed)
{
}

void LootController::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
}

void LootController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

void LootController::TrySpawnLootAt(const glm::vec3& position, const uint32_t spaceshipDanger)
{
    const eDropLootType lootType = PandoraBox::GetInstance()->AttemptToPickLootFromTheBox(spaceshipDanger);
    if (eDropLootType::NONE != lootType) {
        if (const auto lootActor = mCombatActorsPoolHandler->GetFreeLootActor(lootType)) {
            lootActor->SpawnLoot(glm::vec3(position.x, 0.0f, position.z));
        }
    }
}

void LootController::SubscribeSpaceship(const std::shared_ptr<SpaceshipActor>& spaceship)
{
    spaceship->SetOnExplosionCallback(
        [selfWp = weak_from_this(),
         spaceshipDanger = spaceship->GetSpaceshipLevel().GetSpaceshipDanger()](const glm::vec3& position) {
            if (const auto selfSp = selfWp.lock()) {
                selfSp->TrySpawnLootAt(position, spaceshipDanger);
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