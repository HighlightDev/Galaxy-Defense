#pragma once

#include "IMissileFactory.h"

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace Game
{
    class MissileActor;

    class BombMissileFactory
        : public IMissileFactory
    {
        static size_t s_bombBulletCounter;

    public:
        BombMissileFactory() = default;

        virtual std::shared_ptr<MissileActor>
        CreateMissile(const std::shared_ptr<::EngineCore::Scene> &scene,
                      const std::shared_ptr<::EngineCore::Actor> &spawnerActor,
                      const glm::vec3 &translation,
                      const glm::vec3 &rotation,
                      const glm::vec3 &scale) override;
    };
}