#include "LootController.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/LootActor.h"
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
    mCombatActorsPoolHandler->SpawnLoot(10, eLootCategory::CRYSTAL);
}

void LootController::PostPlayLevelFinished()
{
}

void LootController::CleanUp()
{
}

void LootController::Tick(const float deltaTimeSec)
{
    static uint64_t counter = 0;

    if (!mLevelBounds.has_value()) {
        return;
    }
    ++counter;

    if (counter % 300 == 0) {
        if (const auto freeLootActor = mCombatActorsPoolHandler->GetFreeLootActor(eLootCategory::CRYSTAL)) {

            const float x = Random::Float() * 2.0f - 1.0f;
            const float z = Random::Float() * 2.0f - 1.0f;
            const float randomX = mLevelBounds->GetOrigin().x + x * mLevelBounds->GetHalfExtent().x;
            const float randomZ = mLevelBounds->GetOrigin().z + z * mLevelBounds->GetHalfExtent().z;
            freeLootActor->GetRootComponent()->SetTranslation(glm::vec3(randomX, 0.0f, randomZ));
            freeLootActor->SetIsEnabled(true);
        }
    }
}

void LootController::UnpausableTick(const float deltaTimeSec)
{
}

void LootController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

} // namespace Game