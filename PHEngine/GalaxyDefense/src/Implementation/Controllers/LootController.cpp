#include "LootController.h"

#include "Core/GameCore/Scene.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game {

LootController::LootController(const std::shared_ptr<EngineCore::Scene>& scene)
    : mSceneWp(scene)
    , mCombatActorsPoolHandler()
{
}

LootController::~LootController() = default;

void LootController::SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
{
    mCombatActorsPoolHandler = combatActorsPoolHandler;
}

} // namespace Game