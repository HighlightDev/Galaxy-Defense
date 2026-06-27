#include "GravityBombExplosionVisitor.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Implementation/Actors/GravityBombMissileActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Modifiers/GravityModifier.h"

using namespace EngineCore;

namespace Game {
GravityBombExplosionVisitor::GravityBombExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void GravityBombExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& gravityBombMissile = std::static_pointer_cast<GravityBombMissileActor>(ownerSp);
        const auto beforeExplosionActorId = gravityBombMissile->GetCombatActivePhaseActor()->GetObjectId();
        const auto afterExplosionActorId = gravityBombMissile->GetExplosionPhaseActor()->GetObjectId();

        if (missileCollidedActor->GetObjectId() == beforeExplosionActorId) // missile explosion should be triggered
        {
            ownerSp->TriggerExplosion();
        } else if (missileCollidedActor->GetObjectId() == afterExplosionActorId) // black hole effect should be applied
        {
            if (!spaceship->HasModifier(eModifierType::Gravity, ownerSp->GetObjectId())) {
                const auto gravityCenterPosition = missileCollidedActor->GetRootComponent()->GetHierarchyAccumulatedTranslation();
                const auto gravityModifier = std::make_shared<GravityModifier>(spaceship, ownerSp, gravityCenterPosition);
                gravityModifier->SetGravityPower(15.0f);
                gravityModifier->SetTether(gravityBombMissile->GetTetherHostActor(), gravityBombMissile->GetTetherMaterial());
                LogInfo(
                    "GravityBombExplosionVisitor::StartExplosionForSpaceship: |+| gravity from missile ", ownerSp->GetObjectId());
                spaceship->AddModifier(gravityModifier);
            }
        }
    }
}

void GravityBombExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& gravityBombMissile = std::static_pointer_cast<GravityBombMissileActor>(ownerSp);
        const auto beforeExplosionActorId = gravityBombMissile->GetCombatActivePhaseActor()->GetObjectId();
        const auto afterExplosionActorId = gravityBombMissile->GetExplosionPhaseActor()->GetObjectId();

        if (missileCollidedActor->GetObjectId() == beforeExplosionActorId) // nothing to do
        {
        } else if (missileCollidedActor->GetObjectId() == afterExplosionActorId) // black hole effect should be removed
        {
            if (eMissileActivityState::EXPLOSION_FINISHED
                == gravityBombMissile->GetMissileActivityState()) // spaceship should be sucked in to the black hole
            {
                spaceship->TriggerDamageReceived(std::numeric_limits<uint32_t>::max(), ownerSp->GetDamageDealerType());
            } else if (spaceship->HasModifier(eModifierType::Gravity, ownerSp->GetObjectId())) {
                spaceship->RemoveModifier(eModifierType::Gravity, ownerSp->GetObjectId());
                LogInfo(
                    "GravityBombExplosionVisitor::EndExplosionForSpaceship: |-| gravity from missile ", ownerSp->GetObjectId());
            }
        }
    }
}

void GravityBombExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        const auto& gravityBombMissile = std::static_pointer_cast<GravityBombMissileActor>(ownerSp);
        const auto beforeExplosionActorId = gravityBombMissile->GetCombatActivePhaseActor()->GetObjectId();
        const auto afterExplosionActorId = gravityBombMissile->GetExplosionPhaseActor()->GetObjectId();

        if (missileCollidedActor->GetObjectId() == beforeExplosionActorId) // missile explosion should be triggered
        {
            ownerSp->TriggerExplosion();
            spaceObject->TriggerDisabled();
        }
    }
}

void GravityBombExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void GravityBombExplosionVisitor::StartExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void GravityBombExplosionVisitor::EndExplosionForBarrier(
    const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}
} // namespace Game