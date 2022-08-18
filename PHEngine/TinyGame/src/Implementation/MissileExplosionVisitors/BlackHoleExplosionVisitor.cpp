#include "BlackHoleExplosionVisitor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Modifiers/GravityModifier.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Game
{
    BlackHoleExplosionVisitor::BlackHoleExplosionVisitor(const std::shared_ptr<MissileActor> &ownerMissile)
        : MissileExplosionVisitorBase(ownerMissile)
    {
    }

    void BlackHoleExplosionVisitor::StartExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                               const std::shared_ptr<Actor> &missileCollidedActor,
                                                               const std::shared_ptr<Actor> &spaceshipCollidedActor)
    {
        if (const auto &ownerSp = mOwnerWp.lock())
        {
            const auto &blackHoleMissile = std::static_pointer_cast<BlackHoleMissileActor>(ownerSp);
            const auto beforeExplosionActorId = blackHoleMissile->GetCombatActivePhaseActor()->GetObjectId();
            const auto afterExplosionActorId = blackHoleMissile->GetExplosionPhaseActor()->GetObjectId();

            if (missileCollidedActor->GetObjectId() == beforeExplosionActorId) // missile explosion should be triggered
            {
                ownerSp->TriggerExplosion();
            }
            else if (missileCollidedActor->GetObjectId() == afterExplosionActorId) // black hole effect should be applied
            {
                if (!spaceship->HasModifier(eModifierType::Gravity, ownerSp->GetObjectId()))
                {
                    const auto gravityCenterPosition = missileCollidedActor->GetRootComponent()->GetHierarchyAccumulatedTranslation();
                    const auto gravityModifier = std::make_shared<GravityModifiable>(spaceship, ownerSp, gravityCenterPosition);
                    gravityModifier->SetGravityPower(15.0f);
                    LogInfo("BlackHoleExplosionVisitor::StartExplosionForSpaceship => |+| gravity from missile ", ownerSp->GetObjectId());
                    spaceship->AddModifier(gravityModifier);
                }
            }
        }
    }

    void BlackHoleExplosionVisitor::EndExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                             const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                             const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor)
    {
        if (const auto &ownerSp = mOwnerWp.lock())
        {
            const auto &blackHoleMissile = std::static_pointer_cast<BlackHoleMissileActor>(ownerSp);
            const auto beforeExplosionActorId = blackHoleMissile->GetCombatActivePhaseActor()->GetObjectId();
            const auto afterExplosionActorId = blackHoleMissile->GetExplosionPhaseActor()->GetObjectId();

            if (missileCollidedActor->GetObjectId() == beforeExplosionActorId) // nothing to do
            {
            }
            else if (missileCollidedActor->GetObjectId() == afterExplosionActorId) // black hole effect should be removed
            {
                if (eMissileActivityState::EXPLOSION_FINISHED == blackHoleMissile->GetMissileActivityState()) // spaceship should be sucked in to the black hole
                {
                    spaceship->TriggerDisabled();
                }
                else if (spaceship->HasModifier(eModifierType::Gravity, ownerSp->GetObjectId()))
                {
                    spaceship->RemoveModifier(eModifierType::Gravity, ownerSp->GetObjectId());
                    LogInfo("BlackHoleExplosionVisitor::EndExplosionForSpaceship => |-| gravity from missile ", ownerSp->GetObjectId());
                }
            }
        }
    }
}