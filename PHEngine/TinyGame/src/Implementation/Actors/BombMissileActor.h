#pragma once

#include "Core/GameCore/BoundingBox3D.h"

#include "Implementation/Actors/MissileActor.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class MissileExplosionVisitorBase;

    class BombMissileActor
        : public MissileActor
    {
    public:
        BombMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void TriggerSpawn(const glm::vec3 &position) override;

        virtual void TriggerExplosion() override;

        virtual void TriggerExplosionFinished() override;

        virtual void TriggerDisabled() override;

        virtual std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;
    };
}