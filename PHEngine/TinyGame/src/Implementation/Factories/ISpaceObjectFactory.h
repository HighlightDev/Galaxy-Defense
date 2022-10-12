#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class BackgroundSpaceObjectActor;
    class SpaceObjectActor;

    class IBackgroundSpaceObjectFactory
    {
        virtual std::shared_ptr<BackgroundSpaceObjectActor>
        CreateSpaceObject(const std::shared_ptr<::EngineCore::Scene> &scene,
                          const glm::vec3 &translation,
                          const glm::vec3 &rotation,
                          const glm::vec3 &scale,
                          const std::string& imageName,
                          const float billboardSize) = 0;
    };

    class IAsteroidFactory
    {
        virtual std::shared_ptr<SpaceObjectActor>
        CreateSpaceObject(const std::shared_ptr<::EngineCore::Scene> &scene,
                          const glm::vec3 &translation,
                          const glm::vec3 &rotation,
                          const glm::vec3 &scale) = 0;
    };
}