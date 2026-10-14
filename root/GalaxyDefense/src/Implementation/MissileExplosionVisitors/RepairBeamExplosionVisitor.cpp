#include "RepairBeamExplosionVisitor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/RepairBeamActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Modifiers/RepairBeamModifier.h"

using namespace EngineCore;

namespace Game {
RepairBeamExplosionVisitor::RepairBeamExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void RepairBeamExplosionVisitor::StartExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            AddRepairBeamModifier(barrier);
            const auto& repairBeamActorSp = std::dynamic_pointer_cast<RepairBeamActor>(ownerSp);
            if (const auto& spaceStationSp
                = std::dynamic_pointer_cast<SpaceStationActor>(repairBeamActorSp->GetActorWhoSpawnedMeWp().lock())) {
                spaceStationSp->SetIsRayActive(true);
            }
        }
    }
}

void RepairBeamExplosionVisitor::EndExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& repairBeamActorSp = std::dynamic_pointer_cast<RepairBeamActor>(ownerSp);
        if (const auto& spaceStationSp
            = std::dynamic_pointer_cast<SpaceStationActor>(repairBeamActorSp->GetActorWhoSpawnedMeWp().lock())) {
            spaceStationSp->SetIsRayActive(false);
        }
    }
    if (barrier->HasModifier(eModifierType::RepairBeam)) {
        std::dynamic_pointer_cast<RepairBeamModifier>(barrier->GetModifier(eModifierType::RepairBeam))->SetIsExpired(true);
    }
}

void RepairBeamExplosionVisitor::AddRepairBeamModifier(const std::shared_ptr<BarrierActor>& barrier)
{
    if (!barrier->HasModifier(eModifierType::RepairBeam)) {
        barrier->AddModifier(std::make_shared<RepairBeamModifier>(barrier));
    }
}

void RepairBeamExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "RepairBeam should only collide with barriers - invalid StartExplosion call for spaceship");
}

void RepairBeamExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "RepairBeam should only collide with barriers - invalid EndExplosion call for spaceship");
}

void RepairBeamExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "RepairBeam should only collide with barriers - invalid StartExplosion call for space object");
}

void RepairBeamExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
    ext_assert(false, "RepairBeam should only collide with barriers - invalid EndExplosion call for space object");
}
} // namespace Game
