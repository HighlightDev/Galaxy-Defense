#include "BlackHoleExplosionVisitor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Modifiers/GravityModifier.h"
#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game
{
    BlackHoleExplosionVisitor::BlackHoleExplosionVisitor(const std::shared_ptr<MissileActor> &ownerMissile)
        : MissileExplosionVisitorBase(ownerMissile)
    {
    }

    void BlackHoleExplosionVisitor::VisitSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                   const std::shared_ptr<Actor> &missileCollidedActor,
                                                   const std::shared_ptr<Actor> &spaceshipCollidedActor)
    {
        if (const auto &ownerSp = mOwnerWp.lock())
        {
            const auto missileActivityState = ownerSp->GetMissileActivityState();

            if (eMissileActivityState::EXPLOSION == missileActivityState)
            {
                if (!spaceship->HasModifier(eModifierType::Gravity))
                {
                    const auto gravityCenterPosition = missileCollidedActor->GetRootComponent()->GetHierarchyAccumulatedTranslation();
                    const auto gravityModifier =  std::make_shared<GravityModifiable>(spaceship, ownerSp, gravityCenterPosition);
                    gravityModifier->SetGravityPower(5.0f);
                    spaceship->AddModifier(gravityModifier);
                }
            }
            else if (eMissileActivityState::ACTIVE == missileActivityState)
            {
                ownerSp->TriggerExplosion();
            }
        }
    }
}