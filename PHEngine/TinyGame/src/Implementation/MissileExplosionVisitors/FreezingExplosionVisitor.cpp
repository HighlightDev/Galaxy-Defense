#include "FreezingExplosionVisitor.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Modifiers/FreezingModifier.h"

using namespace EngineCore;

namespace Game {
FreezingExplosionVisitor::FreezingExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile)
    : MissileExplosionVisitorBase(ownerMissile)
{
}

void FreezingExplosionVisitor::StartExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            if (!spaceship->HasModifier(eModifierType::Freezing)) {
                const auto freezingModifier = std::make_shared<FreezingModifier>(spaceship);
                freezingModifier->SetFreezingPower(5.0f);
                freezingModifier->SetFreezingTimeout(2.0f);
                LogInfo(
                    "FreezingExplosionVisitor::StartExplosionForSpaceship => |+| freezing from missile ", ownerSp->GetObjectId());
                spaceship->AddModifier(freezingModifier);
            } else {
                LogInfo(
                    "FreezingExplosionVisitor::StartExplosionForSpaceship => Extend freezing from missile ",
                    ownerSp->GetObjectId());
                const auto freezingModifier
                    = std::static_pointer_cast<FreezingModifier>(spaceship->GetModifier(eModifierType::Freezing));
                freezingModifier->ResetFreezingTimer();
            }

            ownerSp->TriggerExplosion();
        }
    }
}

void FreezingExplosionVisitor::EndExplosionForSpaceship(
    const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}

void FreezingExplosionVisitor::StartExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (eMissileActivityState::ACTIVE == ownerSp->GetMissileActivityState()) {
            ownerSp->TriggerExplosion();
            spaceObject->TriggerDisabled();
        }
    }
}

void FreezingExplosionVisitor::EndExplosionForSpaceObject(
    const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
{
}
} // namespace Game