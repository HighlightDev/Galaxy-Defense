#pragma once

#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class MissileActor;
class CombatActorsPoolHandler;

class IMissileFactory {
public:
    virtual std::shared_ptr<MissileActor> CreateMissile(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale)
        = 0;
};
} // namespace Game