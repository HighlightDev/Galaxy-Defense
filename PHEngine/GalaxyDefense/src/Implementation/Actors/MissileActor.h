#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/MissileType.h"
#include "Implementation/DamageDealerType.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class MissileExplosionVisitorBase;
    class CombatActorsPoolHandler;

    enum class eMissileActivityState
    {
        IDLE,
        ACTIVE,
        EXPLOSION,
        EXPLOSION_FINISHED,
        OUT_OF_LEVEL
    };

    class MissileActor
        : public Actor
    {
    protected:
        eMissileActivityState mActivityState{eMissileActivityState::IDLE};

        eMissileType mMissileType{eMissileType::NONE};

        eDamageDealerType mDamageDealerType{eDamageDealerType::NONE};

        std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    public:
        MissileActor(const std::string &gameObjectName,
                     const std::shared_ptr<EngineCore::SceneComponent> &rootComponent,
                     const std::shared_ptr<CombatActorsPoolHandler> &combatActorsPoolHandler);

        virtual bool IsInsideLevel(const BoundingBox3D &boundingBox) const;

        virtual void TriggerSpawn(const glm::vec3 &position,
                                  const glm::vec3 &direction,
                                  const float yawDegrees,
                                  const eDamageDealerType ownerType,
                                  const std::shared_ptr<Actor> &spawnerActor);

        virtual void TriggerExplosion();

        virtual void TriggerExplosionFinished();

        virtual void TriggerDisabled();

        virtual std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor();

        eMissileActivityState GetMissileActivityState() const;

        void SetMissileActivityState(const eMissileActivityState activityState);

        eMissileType GetMissileType() const;

        eDamageDealerType GetDamageDealerType() const;

        std::shared_ptr<CombatActorsPoolHandler> GetCombatActorsPoolHandler() const;
    };
}