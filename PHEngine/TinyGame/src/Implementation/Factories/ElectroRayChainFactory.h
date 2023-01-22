#pragma once

#include "IMissileFactory.h"

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class MissileActor;

    class ElectroRayChainFactory
        : public IMissileFactory
    {
        static size_t s_electroRayChainCounter;

    public:
        ElectroRayChainFactory() = default;

        virtual std::shared_ptr<::Game::MissileActor>
        CreateMissile(const std::shared_ptr<::EngineCore::Scene> &scene,
                      const std::shared_ptr<::EngineCore::Actor> &spawnerActor,
                      const glm::vec3 &translation,
                      const glm::vec3 &rotation,
                      const glm::vec3 &scale) override;
    };
}