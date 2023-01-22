#pragma once

#include "IMissileExplosionChainFactory.h"

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace Game
{
    class ElectroRayChainFactory
        : public IMissileExplosionChainFactory
    {
        static size_t s_electroRayChainCounter;

    public:
        ElectroRayChainFactory() = default;

        virtual std::shared_ptr<::EngineCore::Actor>
        CreateMissileExplosionChain(const std::shared_ptr<::EngineCore::Scene> &scene,
                      const glm::vec3 &translation,
                      const glm::vec3 &rotation,
                      const glm::vec3 &scale) override;
    };
}