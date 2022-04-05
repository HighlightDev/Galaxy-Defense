#pragma once

#include "ISpaceShipFactory.h"

namespace EngineCore
{
    class Actor;
    class Scene;
}

namespace Game
{
    class WeakSpaceShipFactory
        : public ISpaceShipFactory
    {
        static size_t s_weakSpaceShipCounter;

    public:
        WeakSpaceShipFactory() = default;

        virtual std::shared_ptr<::EngineCore::Actor>
        CreateSpaceShip(const std::shared_ptr<::EngineCore::Scene> &scene,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale) override;
    };
}