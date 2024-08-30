#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace Game
{
    class MissileActor;
    class CombatActorsPoolHandler;

    class IMissileFactory
    {
    public:
        virtual std::shared_ptr<MissileActor>
        CreateMissile(const std::shared_ptr<::EngineCore::Scene> &scene,
                      const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
                      const glm::vec3 &translation,
                      const glm::vec3 &rotation,
                      const glm::vec3 &scale) = 0;
    };
}