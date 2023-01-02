#include "BombExplosionVisitor.h"
#include "Core/GameCore/Actor.h"
#include "Core/CommonCore/Random.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

#include <algorithm>

using namespace EngineCore;

#undef min
#undef max

namespace Game
{
    BombExplosionVisitor::BombExplosionVisitor(const std::shared_ptr<MissileActor> &ownerMissile)
        : MissileExplosionVisitorBase(ownerMissile)
    {
    }

    void BombExplosionVisitor::StartExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                          const std::shared_ptr<Actor> &missileCollidedActor,
                                                          const std::shared_ptr<Actor> &spaceshipCollidedActor)
    {
        if (const auto &ownerSp = mOwnerWp.lock())
        {
            if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState())
            {
                const size_t dmg = std::max((size_t)(Random::Float() * 5.0f), 1U);
                spaceship->TriggerDamageReceived(dmg);
                ownerSp->TriggerExplosion();
            }
        }
    }

    void BombExplosionVisitor::EndExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                        const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                        const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor)
    {
    }

    void BombExplosionVisitor::StartExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                            const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                            const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor)
    {

        if (const auto &ownerSp = mOwnerWp.lock())
        {
            if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState())
            {
                spaceObject->TriggerDisabled();
                ownerSp->TriggerExplosion();
            }
        }
    }

    void BombExplosionVisitor::EndExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                          const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                          const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor)
    {
    }
}