#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class MissileExplosionVisitorBase;
    
    enum class eMissileActivityState
    {
        IDLE,
        ACTIVE,
        EXPLOSION
    };

    class MissileActor
        : public Actor
    {
    protected:
        eMissileActivityState mActivityState{eMissileActivityState::IDLE};

    public:
        MissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual bool IsInsideLevel(const BoundingBox &boundingBox) const;

        virtual void TriggerSpawn(const glm::vec3 &position);

        virtual void TriggerExplosion();

        virtual void TriggerDisable();

        virtual std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor();

        eMissileActivityState GetMissileActivityState() const;
    };
}