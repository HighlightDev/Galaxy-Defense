#pragma once

#include "ISpaceObjectFactory.h"

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class SpaceshipActor;

    class AsteroidFactory
        : public IAsteroidFactory
    {
        static size_t s_asteroidCounter;

    public:
        AsteroidFactory() = default;

        virtual std::shared_ptr<SpaceObjectActor>
        CreateSpaceObject(const std::shared_ptr<::EngineCore::Scene> &scene,
                          const glm::vec3 &translation,
                          const glm::vec3 &rotation,
                          const glm::vec3 &scale) override;
    };
}