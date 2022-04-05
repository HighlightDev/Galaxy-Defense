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
    class ISpaceShipFactory
    {
        virtual std::shared_ptr<::EngineCore::Actor>
        CreateSpaceShip(const std::shared_ptr<::EngineCore::Scene> &scene,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale) = 0;
    };
}