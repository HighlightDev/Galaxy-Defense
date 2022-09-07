#pragma once

#include "MissileExplosionVisitorBase.h"

namespace EngineCore
{
    class Actor;
}

namespace Game
{
    class MissileActor;
    class SpaceshipActor;
    class SpaceObjectActor;

    class FreezingExplosionVisitor : public MissileExplosionVisitorBase
    {
    public:
        FreezingExplosionVisitor(const std::shared_ptr<MissileActor> &ownerMissile);

        virtual void StartExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) override;

        virtual void EndExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                              const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                              const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) override;

        virtual void StartExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                  const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                  const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) override;

        virtual void EndExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) override;
    };
}