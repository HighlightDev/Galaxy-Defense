#include "WeakSpaceshipActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/RouteHandlers/WeakSpaceshipRouteHandler.h"
#include "Implementation/Components/UiComponents/SpaceObjectUiComponent.h"
#include "Implementation/DamageDealerType.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

namespace Game {
WeakSpaceshipActor::WeakSpaceshipActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const SpaceshipLevel& spaceshipLevel)
    : SpaceshipActor(gameObjectName, rootComponent, spaceshipLevel)
    , ITweenStateChangeNotifyable()
    , mWeakSpaceshipTweener()
{
    mSpaceshipType = eSpaceshipType::PAWN;
    mRouteHandler = std::make_unique<WeakSpaceshipRouteHandler>(this);
}

void WeakSpaceshipActor::AttachTweener(std::shared_ptr<Tweener> tweener)
{
    ext_assert(tweener, "WeakSpaceshipActor tweener pointer is null");
    LogInfo("WeakSpaceshipActor::AttachTweener: Path to tweener", tweener->GetRelPathTweener());

    Actor::AttachTweener(tweener);
    mWeakSpaceshipTweener = tweener;
    InitTweenerSubscriptions();
}

void WeakSpaceshipActor::InitTweenerSubscriptions()
{
    mWeakSpaceshipTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<WeakSpaceshipActor>(shared_from_this()));
}

void WeakSpaceshipActor::TriggerDamageReceived(const size_t damage, const eDamageDealerType damageDealerType)
{
    if (CheckIsAliveAfterDamage(damage)) {
        mIsDamageEffectActive = true;
        mDamageEffectTimePassed = 0.0f;
        mDamageMessageTimer->RestartTimer();
        mDmgShakeTimer->RestartTimer();
        mUiComponent->SetLabelText(std::to_string(damage));
        mUiComponent->SetHealthBarFillPercent(
            static_cast<float>(mSpaceshipLevel.GetHealth()) / static_cast<float>(mSpaceshipLevel.GetNominalHealth()));
        mUiComponent->FadeIn();
    } else {
        mWeakSpaceshipTweener->ChangeState("s_LifecycleExplosion");
        if (eDamageDealerType::MAIN_PLAYER == damageDealerType) {
            const auto& playerDataProvider = PlayerDataProvider::GetInstance();
            playerDataProvider->SetDestroyedEnemySpaceshipsCount(playerDataProvider->GetDestroyedEnemySpaceshipsCount() + 1);
        }
    }
}

void WeakSpaceshipActor::OnTweenStateChanged(const std::string& stateName)
{
    if ("s_LifecyclePreload" == stateName) {
    } else if ("s_LifecycleActive" == stateName) {
    } else if ("s_LifecycleExplosion" == stateName) {
        TriggerExplosion();
    } else if ("s_LifecycleDestroyed" == stateName) {
        SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
    }
}

void WeakSpaceshipActor::TriggerSpawn(const glm::vec3& position)
{
    mActivityState = eSpaceshipActivityState::ACTIVE;
    SetIsEnabled(true);
    const auto& onRouteMovementComponent = GetOnRouteMovementComponent();
    onRouteMovementComponent->Teleport(position);
    mSpaceshipLevel.RestoreHealth();

    const auto c_light = GetComponentsByType<LightComponent>().back();
    c_light->SetIsVisible(false);

    mWeakSpaceshipTweener->ChangeState("s_LifecycleActive");
}

void WeakSpaceshipActor::TriggerExplosion()
{
    if (mOnExplosionCallback) {
        mOnExplosionCallback(GetWorldPosition());
    }

    const auto& meshes = GetComponentsByType<InstancedStaticMeshComponent>();
    for (const auto& mesh : meshes) {
        mesh->SetIsEnabled(false);
    }

    const auto& c_physics = GetPhysicsComponent();
    c_physics->SetIsEnabled(false);

    const auto c_particles = GetComponentsByType<ParticleSystemBaseComponent>();
    for (const auto& c_particle : c_particles) {
        c_particle->EmitParticles();
    }

    const auto c_lights = GetComponentsByType<LightComponent>();
    for (const auto& c_light : c_lights) {
        c_light->SetIsVisible(true);
    }

    mUiComponent->FadeOut();
    mWeakSpaceshipTweener->ChangeState("s_LifecycleDestroyed");
}

void WeakSpaceshipActor::TriggerDisabled()
{
    SpaceshipActor::TriggerDisabled();
    const auto c_particles = GetComponentsByType<ParticleSystemBaseComponent>();
    for (const auto& c_particle : c_particles) {
        c_particle->ResetParticles();
    }
    mWeakSpaceshipTweener->InitRootState();
    mUiComponent->SetHealthBarFillPercent(1.0f);
}
} // namespace Game