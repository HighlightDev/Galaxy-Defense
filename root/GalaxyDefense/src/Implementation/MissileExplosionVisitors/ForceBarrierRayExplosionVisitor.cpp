#include "ForceBarrierRayExplosionVisitor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/ForceBarrierRayActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Modifiers/ForceBarrierModifier.h"

using namespace EngineCore;

namespace Game {
ForceBarrierRayExplosionVisitor::ForceBarrierRayExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void ForceBarrierRayExplosionVisitor::StartExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            AddForceBarrierModifier(barrier);
            const auto& forceBarrierActorSp = std::dynamic_pointer_cast<ForceBarrierRayActor>(ownerSp);
            if (const auto& spaceStationSp
                = std::dynamic_pointer_cast<SpaceStationActor>(forceBarrierActorSp->GetActorWhoSpawnedMeWp().lock())) {
                spaceStationSp->SetIsRayActive(true);
            }
        }
    }
}

void ForceBarrierRayExplosionVisitor::EndExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& forceBarrierActorSp = std::dynamic_pointer_cast<ForceBarrierRayActor>(ownerSp);
        if (const auto& spaceStationSp
            = std::dynamic_pointer_cast<SpaceStationActor>(forceBarrierActorSp->GetActorWhoSpawnedMeWp().lock())) {
            spaceStationSp->SetIsRayActive(false);
        }
    }
    if (barrier->HasModifier(eModifierType::ForceBarrier)) {
        std::dynamic_pointer_cast<ForceBarrierModifier>(barrier->GetModifier(eModifierType::ForceBarrier))->SetIsExpired(true);
    }
}

void ForceBarrierRayExplosionVisitor::AddForceBarrierModifier(const std::shared_ptr<BarrierActor>& barrier)
{
    if (!barrier->HasModifier(eModifierType::ForceBarrier)) {
        barrier->AddModifier(std::make_shared<ForceBarrierModifier>(barrier));
    }
}

void ForceBarrierRayExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "ForceBarrier should only collide with barriers - invalid StartExplosion call for spaceship");
}

void ForceBarrierRayExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "ForceBarrier should only collide with barriers - invalid EndExplosion call for spaceship");
}

void ForceBarrierRayExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "ForceBarrier should only collide with barriers - invalid StartExplosion call for space object");
}

void ForceBarrierRayExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "ForceBarrier should only collide with barriers - invalid EndExplosion call for space object");
}
} // namespace Game
