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
    class CombatActorsPoolHandler;

    class FreezingMissileActor
        : public MissileActor
    {
    public:
        FreezingMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent,
                             const std::shared_ptr<CombatActorsPoolHandler> &poolHandler);

        void TriggerSpawn(const glm::vec3 &position,
                          const glm::vec3 &direction,
                          const float yawDegrees,
                          const eDamageDealerType ownerType,
                          const std::shared_ptr<Actor> &spawnerActor) override;

        void Tick(const float deltaTime) override;

        void TriggerExplosion() override;

        void TriggerExplosionFinished() override;

        void TriggerDisabled() override;

        std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;
    };
}