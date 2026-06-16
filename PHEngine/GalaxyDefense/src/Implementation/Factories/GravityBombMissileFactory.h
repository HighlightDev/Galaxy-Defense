#pragma once

#include "IMissileFactory.h"

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class MissileActor;
class CombatActorsPoolHandler;

class GravityBombMissileFactory : public IMissileFactory {
    static size_t s_gravityBombMissileCounter;

public:
    GravityBombMissileFactory() = default;

    virtual std::shared_ptr<MissileActor> CreateMissile(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale) override;
};
} // namespace Game