#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class SpaceObjectActor;

    class ISpaceObjectFactory
    {
        virtual std::shared_ptr<SpaceObjectActor>
        CreateSpaceObject(const std::shared_ptr<::EngineCore::Scene> &scene,
                          const glm::vec3 &translation,
                          const glm::vec3 &rotation,
                          const glm::vec3 &scale) = 0;
    };
}