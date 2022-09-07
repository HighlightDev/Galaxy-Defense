#pragma once

#include "SpaceObjectActor.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class AsteroidActor
        : public SpaceObjectActor
    {
    protected:

    public:
        AsteroidActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void TriggerSpawn(const glm::vec3 &position) override;

        virtual void TriggerDisabled() override;
    };
}