#include "WeakSpaceshipActor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/GUI/Text/TextField.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"

namespace Game
{
    WeakSpaceshipActor::WeakSpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : SpaceshipActor(gameObjectName, rootComponent), ITweenStateChangeNotifyable(),
          mWeakSpaceshipTweener()
    {
    }

    void WeakSpaceshipActor::PostLevelInit()
    {
        SpaceshipActor::PostLevelInit();

        mWeakSpaceshipTweener = GetTweenerByName("WeakSpaceshipLifecycle");
        assert(mWeakSpaceshipTweener);
        InitTweenerSubscriptions();
    }

    void WeakSpaceshipActor::InitTweenerSubscriptions()
    {
        mWeakSpaceshipTweener->SubscribeOnStateChange(this);
    }

    void WeakSpaceshipActor::TriggerDamageReceived(const size_t damage)
    {
        if (CheckIsAliveAfterDamage(damage))
        {
            mIsDamageEffectActive = true;
            mDamageEffectTimePassed = 0.0f;
        }
        else
        {
            mWeakSpaceshipTweener->ChangeState("s_LifecycleExplosion");
        }

        mIsDamageTextActive = true;
        mDamageTextTimePassed = 0.0f;

        mDamageTextField->SetText(std::to_string(damage));
        mDamageTextField->SetVisibility(true);
        mDamageTextField->SetPosition(CalculatePositionForDamageText());
    }

    void WeakSpaceshipActor::OnTweenStateChanged(const std::string &stateName)
    {
        // LogInfo("WeakSpaceshipActor::OnTweenStateChanged => New state: ", stateName);

        if ("s_LifecyclePreload" == stateName)
        {
        }
        else if ("s_LifecycleActive" == stateName)
        {
        }
        else if ("s_LifecycleExplosion" == stateName)
        {
            TriggerExplosion();
        }
        else if ("s_LifecycleDestroyed" == stateName)
        {
            TriggerDisabled();
        }
    }

    void WeakSpaceshipActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eSpaceshipActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
        RestoreLife();

        mWeakSpaceshipTweener->ChangeState("s_LifecycleActive");
    }

    void WeakSpaceshipActor::TriggerExplosion()
    {
        const auto c_spaceshipMesh = GetComponentsByType<StaticMeshComponent>().back();
        c_spaceshipMesh->SetIsEnabled(false);

        const auto &c_physics = GetPhysicsComponent();
        c_physics->SetIsEnabled(false);

        const auto c_particle = GetComponentsByType<ParticleSystemComponent>().back();
        c_particle->EmitParticles();
        mWeakSpaceshipTweener->ChangeState("s_LifecycleDestroyed");
    }

    void WeakSpaceshipActor::TriggerDisabled()
    {
        std::for_each(mModifiers.begin(), mModifiers.end(), [](const auto &modifier)
                      { modifier->OnPreRemoved(); });

        mModifiers.clear();
        mActivityState = eSpaceshipActivityState::IDLE;
        mWeakSpaceshipTweener->InitRootState();
    }
}