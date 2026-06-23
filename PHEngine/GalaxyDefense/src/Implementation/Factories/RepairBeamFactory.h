#pragma once

#include "IMissileFactory.h"

namespace EngineCore {
class Scene;
}

namespace Game {
class MissileActor;
class CombatActorsPoolHandler;

// Repair Beam weapon: a pooled ray (like FreezingRay/ElectroRay) that locks onto the nearest barrier. Builds the wavy
// three-layer beam plus nanobots, healing cross and looping pulse; the actor drives targeting in its Tick.
class RepairBeamFactory : public IMissileFactory {
    static size_t s_repairBeamCounter;

public:
    RepairBeamFactory() = default;

    std::shared_ptr<MissileActor> CreateMissile(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale) override;
};
} // namespace Game
