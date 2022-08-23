#pragma once

#include "IMissileFactory.h"

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class MissileActor;

    class BlackHoleMissileFactory
        : public IMissileFactory
    {
        static size_t s_blackHoleMissileCounter;

    public:
        BlackHoleMissileFactory() = default;

        virtual std::shared_ptr<MissileActor>
        CreateMissile(const std::shared_ptr<::EngineCore::Scene> &scene,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale) override;
    };
}