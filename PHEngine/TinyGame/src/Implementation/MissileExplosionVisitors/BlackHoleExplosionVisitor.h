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

    class BlackHoleExplosionVisitor : public MissileExplosionVisitorBase
    {
    public:
        BlackHoleExplosionVisitor(const std::shared_ptr<MissileActor> &ownerMissile);

        virtual void VisitSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                    const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                    const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) override;
    };
}