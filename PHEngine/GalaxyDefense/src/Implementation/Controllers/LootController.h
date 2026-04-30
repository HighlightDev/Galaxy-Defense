#pragma once

#include <memory>

namespace EngineCore {
class Scene;
}

namespace Game {

class CombatActorsPoolHandler;

class LootController {

    std::weak_ptr<EngineCore::Scene> mSceneWp;

    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

public:
    explicit LootController(const std::shared_ptr<EngineCore::Scene>& scene);

    ~LootController();

    void SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);
};

} // namespace Game