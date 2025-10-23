#include "BombExplosionVisitor.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

#include <algorithm>

using namespace EngineCore;

#undef min
#undef max

namespace Game {
BombExplosionVisitor::BombExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void BombExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            const uint32_t dmg = static_cast<uint32_t>(Random::Float() * 7.0f) + 7;
            spaceship->TriggerDamageReceived(dmg, ownerSp->GetDamageDealerType());
            ownerSp->TriggerExplosion();
        }
    }
}

void BombExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void BombExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{

    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            spaceObject->TriggerDisabled();
            ownerSp->TriggerExplosion();
        }
    }
}

void BombExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}
} // namespace Game