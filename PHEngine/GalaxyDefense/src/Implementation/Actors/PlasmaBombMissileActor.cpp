#include "PlasmaBombMissileActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PlasmaTrailComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/PlasmaBombExplosionVisitor.h"

namespace Game {
PlasmaBombMissileActor::PlasmaBombMissileActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
    , mCombatActivePhaseActor()
    , mExplosionSecondPhaseActor()
{
    mMissileType = eMissileType::PLASMA_BOMB;
}

void PlasmaBombMissileActor::CleanUp()
{
    if (mPlasmaMissileTweener) {
        mPlasmaMissileTweener->UnsubscribeFromOnStateChange(std::dynamic_pointer_cast<PlasmaBombMissileActor>(shared_from_this()));
    }

    MissileActor::CleanUp();
}

void PlasmaBombMissileActor::AttachTweener(std::shared_ptr<Tweener> tweener)
{
    ext_assert(tweener, "PlasmaBombMissileActor tweener pointer is null");
    LogInfo("PlasmaBombMissileActor::AttachTweener: Path to tweener", tweener->GetRelPathTweener());

    Actor::AttachTweener(tweener);
    mPlasmaMissileTweener = tweener;
    InitTweenerSubscriptions();
}

void PlasmaBombMissileActor::InitTweenerSubscriptions()
{
    mPlasmaMissileTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<PlasmaBombMissileActor>(shared_from_this()));
}

void PlasmaBombMissileActor::AddCombatActivePhaseActor(const std::shared_ptr<Actor>& combatActivePhaseActor)
{
    ext_assert(!mCombatActivePhaseActor, "PlasmaBombMissileActor combat active phase actor already exists");
    mCombatActivePhaseActor = combatActivePhaseActor;
    AddChild(combatActivePhaseActor);
}

void PlasmaBombMissileActor::AddExplosionSecondPhaseActor(const std::shared_ptr<MissileActor>& explosionSecondPhaseActor)
{
    ext_assert(!mExplosionSecondPhaseActor, "PlasmaBombMissileActor explosion second phase actor already exists");
    mExplosionSecondPhaseActor = explosionSecondPhaseActor;
    AddChild(mExplosionSecondPhaseActor);
}

const std::shared_ptr<Actor>& PlasmaBombMissileActor::GetCombatActivePhaseActor() const
{
    return mCombatActivePhaseActor;
}

const std::shared_ptr<MissileActor>& PlasmaBombMissileActor::GetExplosionPhaseActor() const
{
    return mExplosionSecondPhaseActor;
}

void PlasmaBombMissileActor::SetTrailComponent(const std::shared_ptr<EngineCore::PlasmaTrailComponent>& trailComponent)
{
    mTrailComponent = trailComponent;
}

void PlasmaBombMissileActor::SetBurnParticleComponent(
    const std::shared_ptr<EngineCore::CpuParticleSystemComponent>& burnParticleComponent)
{
    mBurnParticleComponent = burnParticleComponent;
}

void PlasmaBombMissileActor::OnTweenStateChanged(const std::string& stateName)
{
    if ("s_FirstPhasePreload" == stateName) {
    } else if ("s_FirstPhaseActiveCombat" == stateName) {
    } else if ("s_Impact" == stateName) {
        // The projectile has just vanished. Move the explosion actor (which carries the plasma sphere) to the impact
        // point and force its transform to recompute now, so the sphere pops up where the projectile struck rather than
        // at its previous position (UpdateTransform already ran this frame, before the tween tick).
        const auto impactPosition = mCombatActivePhaseActor->GetRootComponent()->GetHierarchyAccumulatedTranslation();
        mExplosionSecondPhaseActor->GetRootComponent()->SetTranslation(impactPosition);
        mExplosionSecondPhaseActor->UpdateTransform(true);

        // Spray scorch sparks from the impact point. Emitted after the explosion actor (which owns the burst) was moved
        // to the impact position above, so the particles spawn where the plasma struck.
        if (mBurnParticleComponent) {
            mBurnParticleComponent->EmitParticles();
        }

        // Freeze the flight trail at the moment of impact and start its smooth fade: it stops recording new points and
        // tapers its width to nothing over the Impact->Dissipate window, disabling itself once invisible.
        if (mTrailComponent) {
            mTrailComponent->StartFadeOut(Constants::PlasmaBombMissile::c_trailFadeOutSec);
        }

        TriggerLifecycle_Dissipate();
    } else if ("s_Dissipate" == stateName) {
        TriggerDisabled();
    } else {
        ext_assert(false, "PlasmaBombMissileActor unknown tweener state");
    }
}

void PlasmaBombMissileActor::TriggerLifecycle_FirstPhaseActiveCombat()
{
    ext_assert(mPlasmaMissileTweener, "PlasmaBombMissileActor tweener is null in FirstPhaseActiveCombat");
    mPlasmaMissileTweener->ChangeState("s_FirstPhaseActiveCombat");
}

void PlasmaBombMissileActor::TriggerLifecycle_Impact()
{
    ext_assert(mPlasmaMissileTweener, "PlasmaBombMissileActor tweener is null in Impact");
    mPlasmaMissileTweener->ChangeState("s_Impact");
}

void PlasmaBombMissileActor::TriggerLifecycle_Dissipate()
{
    ext_assert(mPlasmaMissileTweener, "PlasmaBombMissileActor tweener is null in Dissipate");
    mPlasmaMissileTweener->ChangeState("s_Dissipate");
}

void PlasmaBombMissileActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mActivityState = eMissileActivityState::ACTIVE;
    SetIsEnabled(true);
    const auto& existingRotation = mCombatActivePhaseActor->GetRootComponent()->GetAdditionalRotation();
    mCombatActivePhaseActor->GetRootComponent()->SetAdditionalRotation(
        glm::vec3(existingRotation.x, yawDegrees, existingRotation.z));
    mCombatActivePhaseActor->GetMovementComponent()->SetDirection(direction);
    mCombatActivePhaseActor->GetMovementComponent()->Teleport(position);
    mSpawnerActor = spawnerActor;

    // Start a fresh flight trail (the missile is pooled, so drop any path left from a previous launch) and switch it on.
    if (mTrailComponent) {
        mTrailComponent->ResetTrail();
        mTrailComponent->SetIsEnabled(true);
    }

    TriggerLifecycle_FirstPhaseActiveCombat();
}

void PlasmaBombMissileActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    const auto c_soundList = GetComponentsByType<SoundComponent>();
    ext_assert(c_soundList.size(), "PlasmaBombMissileActor has no sound components for explosion");
    c_soundList.back()->PlayBuffer("explosion");
    TriggerLifecycle_Impact();
}

void PlasmaBombMissileActor::TriggerExplosionFinished()
{
    // Not part of the plasma flow (the Impact->Dissipate tween drives the rest); kept for the MissileActor interface.
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
}

void PlasmaBombMissileActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;

    // By now the trail has tapered to nothing via its fade-out (started on impact) and disabled itself; force it off too
    // in case the missile was disabled early, so a reused trail never shows a leftover scar.
    if (mTrailComponent) {
        mTrailComponent->SetIsRecording(false);
        mTrailComponent->SetIsEnabled(false);
    }

    mPlasmaMissileTweener->InitRootState();
    SetIsEnabled(false);
}

bool PlasmaBombMissileActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(
        boundingBox.GetMin(), boundingBox.GetMax(), mCombatActivePhaseActor->GetRootComponent()->GetTranslation());
}

std::shared_ptr<MissileExplosionVisitorBase> PlasmaBombMissileActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<PlasmaBombExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
}
} // namespace Game
