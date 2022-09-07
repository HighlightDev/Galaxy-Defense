#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    enum class eSpaceObjectActivityState
    {
        IDLE,
        ACTIVE
    };

    class SpaceObjectActor
        : public Actor
    {
    protected:
        eSpaceObjectActivityState mActivityState{eSpaceObjectActivityState::IDLE};

    public:
        SpaceObjectActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual bool IsInsideLevel(const BoundingBox &boundingBox) const;

        virtual void TriggerSpawn(const glm::vec3 &position) = 0;

        virtual void TriggerDisabled() = 0;

        eSpaceObjectActivityState GetActivityState() const;
    };
}