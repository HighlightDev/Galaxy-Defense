#include "GravityModifier.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

#include <glm/common.hpp>

#include <limits>
#include <string>

// Windows headers pulled in transitively define min/max macros that break std::numeric_limits<...>::max().
#undef min
#undef max

namespace Game {
namespace {
// Distance to the singularity at which the ship starts/finishes stretching.
constexpr float c_spaghettiStartDistance = 6.0f;
constexpr float c_spaghettiFullDistance = 3.0f;
// Scale multipliers at full spaghettification: elongate along the pull axis, squash perpendicular.
constexpr float c_spaghettiStretchAlongPull = 2.5f;
constexpr float c_spaghettiSquashPerpendicular = 0.4f;
// Distance to the core within which a collapsing singularity consumes (destroys) the captured ship.
constexpr float c_consumeDistance = 2.0f;
} // namespace

GravityModifier::GravityModifier(
    const std::weak_ptr<SpaceshipActor>& owner,
    const std::weak_ptr<MissileActor>& missile,
    const glm::vec3& gravityCenterPosition)
    : mOwnerWp(owner)
    , mMissileWp(missile)
    , mGravityCenterPosition(gravityCenterPosition)
    , mGravityPower(1.0f)
{
}

eModifierType GravityModifier::GetModifierType() const
{
    return eModifierType::Gravity;
}

int32_t GravityModifier::CreatorObjectId() const
{
    // The creator is the missile that produced the gravity well; the explosion visitor adds/queries/removes the
    // modifier by the missile's object id, so this must match it (returning the ship id breaks HasModifier/RemoveModifier).
    const auto& missileSp = mMissileWp.lock();
    ext_assert(missileSp, "GravityModifier missile pointer is null");
    return missileSp->GetObjectId();
}

void GravityModifier::Tick(const float deltaTimeSec)
{
    const auto& spaceshipSp = mOwnerWp.lock();
    const auto& missileSp = mMissileWp.lock();

    if (spaceshipSp && missileSp) {
        const auto spaceshipPosition = spaceshipSp->GetRootComponent()->GetTranslation();
        const auto toGravityCenterVec = mGravityCenterPosition - spaceshipPosition;
        const auto vecLength = glm::length(toGravityCenterVec);
        if (vecLength > 0.001f) // check if length of vector is not zero otherwise normalized vector will be NaN
        {
            const auto nToGravityCenter = toGravityCenterVec / vecLength;
            spaceshipSp->GetMovementComponent()->Move(nToGravityCenter * mGravityPower, deltaTimeSec);
        }

        ApplySpaghettification(spaceshipSp, vecLength);

        // Once the singularity is collapsing, consume ships that have reached the core: trigger their death so they
        // explode, transition to PENDING_DISABLE and are returned to the pool. Disabling the ship removes this modifier
        // via RemoveAllModifiers, which restores the original scale in OnPreRemoved.
        if (!mHasConsumedOwner && eMissileActivityState::EXPLOSION_FINISHED == missileSp->GetMissileActivityState()
            && vecLength < c_consumeDistance) {
            spaceshipSp->TriggerDamageReceived(std::numeric_limits<uint32_t>::max(), missileSp->GetDamageDealerType());
            mHasConsumedOwner = true;
        }

        if (mHasConsumedOwner) {
            DestroyTether();
        } else {
            UpdateTether(spaceshipSp, spaceshipPosition);
        }
    }
}

void GravityModifier::ApplySpaghettification(const std::shared_ptr<SpaceshipActor>& spaceship, const float distanceToCenter)
{
    const auto& rootComponent = spaceship->GetRootComponent();
    if (!rootComponent) {
        return;
    }

    if (!mIsScaleCaptured) {
        mOriginalScale = rootComponent->GetScale();
        mIsScaleCaptured = true;
    }

    // 0 while far away, 1 once the ship reaches the singularity core.
    const float spaghettiAmount = glm::clamp(
        (c_spaghettiStartDistance - distanceToCenter) / (c_spaghettiStartDistance - c_spaghettiFullDistance), 0.0f, 1.0f);

    // Ships are pulled along (and face) their local forward axis (z), so stretch z and squash x/y.
    const glm::vec3 spaghettiScale(
        EngineMath::LerpNormalizedFloat(1.0f, c_spaghettiSquashPerpendicular, spaghettiAmount),
        EngineMath::LerpNormalizedFloat(1.0f, c_spaghettiSquashPerpendicular, spaghettiAmount),
        EngineMath::LerpNormalizedFloat(1.0f, c_spaghettiStretchAlongPull, spaghettiAmount));

    rootComponent->SetScale(mOriginalScale * spaghettiScale);
}

void GravityModifier::RestoreOriginalScale()
{
    if (!mIsScaleCaptured) {
        return;
    }

    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        if (const auto& rootComponent = spaceshipSp->GetRootComponent()) {
            rootComponent->SetScale(mOriginalScale);
        }
    }
    mIsScaleCaptured = false;
}

void GravityModifier::OnPreRemoved()
{
    RestoreOriginalScale();
    DestroyTether();
}

void GravityModifier::SetTether(
    const std::shared_ptr<EngineCore::Actor>& hostActor, const std::shared_ptr<Graphics::IMaterial>& material)
{
    mTetherHostWp = hostActor;
    mTetherMaterial = material;
}

void GravityModifier::UpdateTether(const std::shared_ptr<SpaceshipActor>& spaceship, const glm::vec3& shipPosition)
{
    const auto hostActorSp = mTetherHostWp.lock();
    if (!hostActorSp || !mTetherMaterial) {
        return;
    }

    if (!mTetherComponent) {
        const auto sceneSp = spaceship->GetSceneOwner().lock();
        if (!sceneSp) {
            return;
        }

        const auto& tetherCreator
            = std::make_shared<EngineCore::RuntimeGeneratedMeshComponentCreator<EngineCore::RuntimeGeneratedQuadraticBezierCurveComponent>>();
        const auto d_tether = std::make_shared<EngineCore::RuntimeGeneratedMeshComponentData>(
            "c_gravityBombTether_" + std::to_string(spaceship->GetObjectId()),
            static_cast<size_t>(64),
            glm::vec3(),
            glm::vec3(),
            glm::vec3(1),
            mTetherMaterial,
            true,
            true);
        mTetherComponent = std::static_pointer_cast<EngineCore::RuntimeGeneratedQuadraticBezierCurveComponent>(
            sceneSp->CreateComponent_GameThread(tetherCreator, d_tether));
        mTetherComponent->SetCanBloomBeApplied(true);
        mTetherComponent->SetLineWidth(0.25f);
        mTetherComponent->SetCurveSegmentsCount(12);
        hostActorSp->AddComponent(mTetherComponent);
    }

    // Bow the curve away from the straight line so the tether reads as a curve being drawn into the singularity.
    const glm::vec3 midPoint = (shipPosition + mGravityCenterPosition) * 0.5f;
    const glm::vec3 controlPoint = midPoint + glm::vec3(0.0f, 2.0f, 0.0f);
    mTetherComponent->SetLineBeginWorldSpacePosition(shipPosition);
    mTetherComponent->SetLineEndWorldSpacePosition(mGravityCenterPosition);
    mTetherComponent->SetBezierControlPointWorldSpacePosition(controlPoint);
}

void GravityModifier::DestroyTether()
{
    if (mTetherComponent) {
        if (const auto hostActorSp = mTetherHostWp.lock()) {
            // Same pattern as BarrierActor: Actor::RemoveComponent first (drops it from the actor and clears its owner),
            // then Scene::RemoveComponent removes the primitive scene proxy from the renderer and unregisters it. The
            // cleared owner stops Scene::RemoveComponent from calling back into the actor.
            hostActorSp->RemoveComponent(mTetherComponent);
            if (const auto sceneSp = hostActorSp->GetSceneOwner().lock()) {
                sceneSp->RemoveComponent(mTetherComponent);
            }
        }
        mTetherComponent.reset();
    }
}

bool GravityModifier::IsExpired() const
{
    // Stop pulling (and let OnPreRemoved restore the ship scale) once the missile/black hole is gone or has finished
    // its whole lifecycle. This is the catch-all that guarantees spaghettified ships are restored.
    const auto& missileSp = mMissileWp.lock();
    if (!missileSp) {
        return true;
    }
    return eMissileActivityState::IDLE == missileSp->GetMissileActivityState();
}

void GravityModifier::SetGravityPower(const float gravityPower)
{
    mGravityPower = gravityPower;
}
} // namespace Game