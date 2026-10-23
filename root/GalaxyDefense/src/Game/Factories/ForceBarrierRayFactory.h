#pragma once

#include "IMissileFactory.h"

namespace EngineCore {
class Scene;
}

namespace Game {
class MissileActor;
class CombatActorsPoolHandler;

class ForceBarrierRayFactory : public IMissileFactory {
    static size_t s_forceBarrierCounter;

public:
    ForceBarrierRayFactory() = default;

    std::shared_ptr<MissileActor> CreateMissile(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale) override;
};
} // namespace Game
