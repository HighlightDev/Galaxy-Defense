#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ITickable.h"
#include "ILevelController.h"

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace EngineCore {
class Scene;
}

namespace Game {

class CombatActorsPoolHandler;
class SpaceshipActor;

class LootController : public ILevelController, public ITickable, public std::enable_shared_from_this<LootController> {

    std::weak_ptr<EngineCore::Scene> mSceneWp;

    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    std::optional<EngineCore::BoundingBox3D> mLevelBounds;

public:
    explicit LootController(const std::weak_ptr<EngineCore::Scene>& scene);

    ~LootController();

    void SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void SetLevelBounds(const EngineCore::BoundingBox3D& levelBounds);

    void TrySpawnLootAt(const glm::vec3& position);

    void SubscribeSpaceship(const std::shared_ptr<SpaceshipActor>& spaceship);

    void SubscribeToEnemySpaceships(const std::vector<std::shared_ptr<SpaceshipActor>>& spaceships);
};

} // namespace Game