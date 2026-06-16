#include "GravityBombMissileActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/GravityGridComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/GravityBombExplosionVisitor.h"

namespace Game {
GravityBombMissileActor::GravityBombMissileActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
    , mCombatActivePhaseActor()
    , mExplosionSecondPhaseActor()
{
    mMissileType = eMissileType::BLACK_HOLE;
}

void GravityBombMissileActor::CleanUp()
{
    if (mBlackMissileTweener) {
        mBlackMissileTweener->UnsubscribeFromOnStateChange(std::dynamic_pointer_cast<GravityBombMissileActor>(shared_from_this()));
    }

    MissileActor::CleanUp();
}

void GravityBombMissileActor::AttachTweener(std::shared_ptr<Tweener> tweener)
{
    ext_assert(tweener, "GravityBombMissileActor tweener pointer is null");
    LogInfo("GravityBombMissileActor::AttachTweener: Path to tweener", tweener->GetRelPathTweener());

    Actor::AttachTweener(tweener);
    mBlackMissileTweener = tweener;
    InitTweenerSubscriptions();
}

void GravityBombMissileActor::InitTweenerSubscriptions()
{
    mBlackMissileTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<GravityBombMissileActor>(shared_from_this()));
}

void GravityBombMissileActor::AddCombatActivePhaseActor(const std::shared_ptr<Actor>& combatActivePhaseActor)
{
    ext_assert(!mCombatActivePhaseActor, "GravityBombMissileActor combat active phase actor already exists");
    mCombatActivePhaseActor = combatActivePhaseActor;
    AddChild(combatActivePhaseActor);
}

void GravityBombMissileActor::AddExplosionSecondPhaseActor(const std::shared_ptr<MissileActor>& explosionSecondPhaseActor)
{
    ext_assert(!mExplosionSecondPhaseActor, "GravityBombMissileActor explosion second phase actor already exists");
    mExplosionSecondPhaseActor = explosionSecondPhaseActor;
    AddChild(mExplosionSecondPhaseActor);
}

const std::shared_ptr<Actor>& GravityBombMissileActor::GetCombatActivePhaseActor() const
{
    return mCombatActivePhaseActor;
}

const std::shared_ptr<MissileActor>& GravityBombMissileActor::GetExplosionPhaseActor() const
{
    return mExplosionSecondPhaseActor;
}

void GravityBombMissileActor::SetTetherHostActor(const std::shared_ptr<Actor>& hostActor)
{
    mTetherHostActor = hostActor;
}

const std::shared_ptr<Actor>& GravityBombMissileActor::GetTetherHostActor() const
{
    return mTetherHostActor;
}

void GravityBombMissileActor::SetTetherMaterial(const std::shared_ptr<Graphics::IMaterial>& material)
{
    mTetherMaterial = material;
}

const std::shared_ptr<Graphics::IMaterial>& GravityBombMissileActor::GetTetherMaterial() const
{
    return mTetherMaterial;
}

void GravityBombMissileActor::OnTweenStateChanged(const std::string& stateName)
{
    if ("s_FirstPhasePreload" == stateName) {
    } else if ("s_FirstPhaseActiveCombat" == stateName) {
    } else if ("s_Impact" == stateName) {
        // Singularity spawns at zero size and grows over the Impact->GravityCapture transition. The orbital rings are
        // components of the singularity actor, so they inherit its transform (position + scale) automatically.
        mExplosionSecondPhaseActor->TriggerSpawn(
            mCombatActivePhaseActor->GetRootComponent()->GetTranslation(), {}, 0.0f, mDamageDealerType, mSpawnerActor);
        // Two particle systems live on the singularity actor (see GravityBombMissileFactory, in this order): [0] accretion
        // swirl, emitted now on impact; [1] collapse burst, emitted later on s_Collapse.
        mExplosionSecondPhaseActor->GetComponentsByType<CpuParticleSystemComponent>().front()->EmitParticles();

        // Anchor and switch on the gravity-well grid. It lives on the identity tether host (world-space geometry), so it
        // starts disabled and is only enabled/positioned here on impact.
        if (const auto& tetherHost = GetTetherHostActor()) {
            const auto gridCenter = mCombatActivePhaseActor->GetRootComponent()->GetHierarchyAccumulatedTranslation();
            for (const auto& gravityGrid : tetherHost->GetComponentsByType<GravityGridComponent>()) {
                gravityGrid->SetGridCenterWorldSpacePosition(gridCenter);
                gravityGrid->SetIsEnabled(true);
            }
        }

        TriggerLifecycle_GravityCapture();
    } else if ("s_GravityCapture" == stateName) {
        // Full-size singularity holds while gravity pulls ships in (GravityCapture->Collapse window).
        TriggerLifecycle_Collapse();
    } else if ("s_Collapse" == stateName) {
        // Capture window elapsed: fire the radial collapse burst (second particle system), then commit to destroying the
        // captured ships and collapse + fade out.
        mExplosionSecondPhaseActor->GetComponentsByType<CpuParticleSystemComponent>().back()->EmitParticles();
        TriggerExplosionFinished();
    } else if ("s_Fade" == stateName) {
        TriggerLifecycle_GravityBombSuckIn();
    } else if ("s_GravityBombSuckIn" == stateName) {
        TriggerDisabled();
    } else {
        ext_assert(false, "GravityBombMissileActor unknown tweener state");
    }
}

void GravityBombMissileActor::TriggerLifecycle_FirstPhasePreload()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in FirstPhasePreload");
    mBlackMissileTweener->ChangeState("s_FirstPhasePreload");
}

void GravityBombMissileActor::TriggerLifecycle_FirstPhaseActiveCombat()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in FirstPhaseActiveCombat");
    mBlackMissileTweener->ChangeState("s_FirstPhaseActiveCombat");
}

void GravityBombMissileActor::TriggerLifecycle_Impact()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in Impact");
    mBlackMissileTweener->ChangeState("s_Impact");
}

void GravityBombMissileActor::TriggerLifecycle_GravityCapture()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in GravityCapture");
    mBlackMissileTweener->ChangeState("s_GravityCapture");
}

void GravityBombMissileActor::TriggerLifecycle_Collapse()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in Collapse");
    mBlackMissileTweener->ChangeState("s_Collapse");
}

void GravityBombMissileActor::TriggerLifecycle_Fade()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in Fade");
    mBlackMissileTweener->ChangeState("s_Fade");
}

void GravityBombMissileActor::TriggerLifecycle_GravityBombSuckIn()
{
    ext_assert(mBlackMissileTweener, "GravityBombMissileActor tweener is null in GravityBombSuckIn");
    mBlackMissileTweener->ChangeState("s_GravityBombSuckIn");
}

void GravityBombMissileActor::TriggerSpawn(
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

    TriggerLifecycle_FirstPhaseActiveCombat();
}

void GravityBombMissileActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    const auto c_soundList = GetComponentsByType<SoundComponent>();
    ext_assert(c_soundList.size(), "GravityBombMissileActor has no sound components for explosion");
    c_soundList.back()->PlayBuffer("explosion");
    TriggerLifecycle_Impact();
}

void GravityBombMissileActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
    TriggerLifecycle_Fade();
}

void GravityBombMissileActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;

    // The gravity-well grid lives on the identity tether host and is not scaled down by the tween (unlike the sphere and
    // rings), so it must be switched off explicitly here or it stays visible after the effect ends.
    if (const auto& tetherHost = GetTetherHostActor()) {
        for (const auto& gravityGrid : tetherHost->GetComponentsByType<GravityGridComponent>()) {
            gravityGrid->SetIsEnabled(false);
        }
    }

    mBlackMissileTweener->InitRootState();
    SetIsEnabled(false);
}

bool GravityBombMissileActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(
        boundingBox.GetMin(), boundingBox.GetMax(), mCombatActivePhaseActor->GetRootComponent()->GetTranslation());
}

std::shared_ptr<MissileExplosionVisitorBase> GravityBombMissileActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<GravityBombExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
}
} // namespace Game