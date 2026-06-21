#include "PlasmaBombExplosionVisitor.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Modifiers/PlasmaModifier.h"

using namespace EngineCore;

namespace Game {
PlasmaBombExplosionVisitor::PlasmaBombExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void PlasmaBombExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        // Single projectile: on hit, brand the ship with a plasma burn (damage-over-time), then detonate (the projectile
        // vanishes and the radial burst lights up at the impact point).
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            if (!spaceship->HasModifier(eModifierType::Plasma, ownerSp->GetObjectId())) {
                const auto plasmaModifier = std::make_shared<PlasmaModifier>(spaceship, ownerSp);
                plasmaModifier->SetDamagePerSecond(Constants::PlasmaBombMissile::c_dotDamagePerSecond);
                plasmaModifier->SetDurationSec(Constants::PlasmaBombMissile::c_dotDurationSec);
                spaceship->AddModifier(plasmaModifier);
                LogInfo("PlasmaBombExplosionVisitor::StartExplosionForSpaceship: |+| plasma burn from missile ", ownerSp->GetObjectId());
            }
            ownerSp->TriggerExplosion();
        }
    }
}

void PlasmaBombExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
}

void PlasmaBombExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            spaceObject->TriggerDisabled();
            ownerSp->TriggerExplosion();
        }
    }
}

void PlasmaBombExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<Actor>& missileCollidedActor)
{
}
} // namespace Game
