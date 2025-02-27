#pragma once

#include "IMissileFactory.h"

namespace EngineCore {
class Scene;
}

namespace Game {
class MissileActor;
class CombatActorsPoolHandler;

class FreezingRayFactory : public IMissileFactory {
    static size_t s_rayCounter;

public:
    FreezingRayFactory() = default;

    virtual std::shared_ptr<MissileActor> CreateMissile(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const float hitRadius) override;
};
} // namespace Game