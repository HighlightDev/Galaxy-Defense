#pragma once

#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace EngineCore {
class SceneComponent;
}

namespace Game {

class CombatActorsPoolHandler;
class NavigationController;

class PortalActor : public Actor {

    std::weak_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandlerWp;

    std::weak_ptr<NavigationController> mNavigationControllerWp;

    float mAccumulatedDeltaTime{0.0f};

    float mSpawnInterval{0.0f};

    bool mIsSpawnActive{false};

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mColorIntensity;

public:
    PortalActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTimeSec) override;

    void SetCombatActorsPoolsHandler(const std::weak_ptr<CombatActorsPoolHandler>& poolHandlerWp);

    void SetNavigationController(const std::weak_ptr<NavigationController>& navigationControllerWp);

    void SetupSpaceshipSpawn(const size_t spawnIntervalMilliseconds);

    void SetSpawnState(const bool isActive);

    bool IsSpawnActive() const;

    void SetColorIntensity(const glm::vec3& colorIntensity);
};
} // namespace Game