#include "FreezingRayExplosionVisitor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Game/Actors/FreezingRayActor.h"
#include "Game/Actors/MissileActor.h"
#include "Game/Actors/SpaceObjectActor.h"
#include "Game/Actors/SpaceStationActor.h"
#include "Game/Actors/SpaceshipActor.h"
#include "Game/Modifiers/FreezingRayModifier.h"

#include <algorithm>

using namespace EngineCore;

#undef min
#undef max

namespace Game {
FreezingRayExplosionVisitor::FreezingRayExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void FreezingRayExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            AddFreezingModifier(spaceship);
            const auto& freezingRayActorSp = std::dynamic_pointer_cast<FreezingRayActor>(ownerSp);
            if (const auto& spaceStationSp
                = std::dynamic_pointer_cast<SpaceStationActor>(freezingRayActorSp->GetActorWhoSpawnedMeWp().lock())) {
                spaceStationSp->SetIsRayActive(true);
            }
        }
    }
}

void FreezingRayExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& freezingRayActorSp = std::dynamic_pointer_cast<FreezingRayActor>(ownerSp);
        if (const auto& spaceStationSp
            = std::dynamic_pointer_cast<SpaceStationActor>(freezingRayActorSp->GetActorWhoSpawnedMeWp().lock())) {
            spaceStationSp->SetIsRayActive(false);
        }
    }
    if (spaceship->HasModifier(eModifierType::FreezingRay)) {
        std::dynamic_pointer_cast<FreezingRayModifier>(spaceship->GetModifier(eModifierType::FreezingRay))->SetIsExpired(true);
    }
}

void FreezingRayExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    // this function should not be called, as freezing ray should not collide with space objects
    ext_assert(false, "FreezingRay should not collide with space objects - invalid StartExplosion call");
}

void FreezingRayExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    // this function should not be called, as freezing ray should not collide with space objects
    ext_assert(false, "FreezingRay should not collide with space objects - invalid EndExplosion call");
}

void FreezingRayExplosionVisitor::StartExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void FreezingRayExplosionVisitor::EndExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void FreezingRayExplosionVisitor::AddFreezingModifier(const std::shared_ptr<SpaceshipActor>& spaceship)
{
    if (!spaceship->HasModifier(eModifierType::FreezingRay)) {
        const auto freezingModifier = std::make_shared<FreezingRayModifier>(spaceship);
        freezingModifier->SetFreezingPower(5.0f);
        spaceship->AddModifier(freezingModifier);
    }
}
} // namespace Game