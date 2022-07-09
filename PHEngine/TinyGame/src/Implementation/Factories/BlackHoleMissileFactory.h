#pragma once

#include "IMissileFactory.h"

namespace EngineCore
{
    class Actor;
    class Scene;
}

namespace Game
{
    class BlackHoleMissileFactory
        : public IMissileFactory
    {
        static size_t s_blackHoleMissileCounter;

    public:
        BlackHoleMissileFactory() = default;

        virtual std::shared_ptr<::EngineCore::Actor>
        CreateWeaponBullet(const std::shared_ptr<::EngineCore::Scene> &scene,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale) override;
    };
}