#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Actor;
    class Scene;
}

namespace Game
{
    class IMissileFactory
    {
        virtual std::shared_ptr<::EngineCore::Actor>
        CreateWeaponBullet(const std::shared_ptr<::EngineCore::Scene> &scene,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale) = 0;
    };
}