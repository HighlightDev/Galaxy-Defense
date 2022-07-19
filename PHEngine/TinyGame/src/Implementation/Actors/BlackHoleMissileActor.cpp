#include "BlackHoleMissileActor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace Game
{
    BlackHoleMissileActor::BlackHoleMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : MissileActor(gameObjectName, rootComponent),
          mCombatActivePhaseActor(),
          mExplosionSecondPhaseActor()
    {
    }

    void BlackHoleMissileActor::AttachTweener(std::shared_ptr<Tweener> tweener)
    {
        assert(tweener);
        LogInfo("BlackHoleMissileActor::AttachTweener => Path to tweener", tweener->GetRelPathTweener());

        Actor::AttachTweener(tweener);
        InitTweenerSubscriptions();
    }

    void BlackHoleMissileActor::InitTweenerSubscriptions()
    {
        mTweener->SubscribeOnStateChange(this);
    }

    void BlackHoleMissileActor::AddCombatActivePhaseActor(const std::shared_ptr<Actor> &combatActivePhaseActor)
    {
        assert(!mCombatActivePhaseActor);
        mCombatActivePhaseActor = combatActivePhaseActor;
        AddChild(combatActivePhaseActor);
    }

    void BlackHoleMissileActor::AddExplosionSecondPhaseActor(const std::shared_ptr<Actor> &explosionSecondPhaseActor)
    {
        assert(!mExplosionSecondPhaseActor);
        mExplosionSecondPhaseActor = explosionSecondPhaseActor;
        AddChild(mExplosionSecondPhaseActor);
    }

    void BlackHoleMissileActor::OnTweenStateChanged(const std::string &stateName)
    {
        LogInfo("BlackHoleMissileActor::OnTweenStateChanged => New state: ", stateName);

        if ("s_FirstPhasePreload" == stateName)
        {
        }
        else if ("s_FirstPhaseActiveCombat" == stateName)
        {
        }
        else if ("s_FirstPhaseExplosion" == stateName)
        {
            mExplosionSecondPhaseActor->GetMovementComponent()->Teleport(mCombatActivePhaseActor->GetRootComponent()->GetTranslation());
            const auto c_particle = mExplosionSecondPhaseActor->GetComponentsByType<ParticleSystemComponent>().back();
            c_particle->EmitParticles(100);
            TriggerLifecycle_SecondPhaseExplosion();
        }
        else if ("s_SecondPhaseExplosion" == stateName)
        {
            TriggerDisable();
        }
        else
        {
            assert(false);
        }
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhasePreload()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_FirstPhasePreload");
        assert(mTweener);

        mTweener->ChangeState("s_FirstPhasePreload");
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhaseActiveCombat()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_FirstPhaseActiveCombat");
        assert(mTweener);

        mTweener->ChangeState("s_FirstPhaseActiveCombat");
    }

    void BlackHoleMissileActor::TriggerLifecycle_FirstPhaseExplosion()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_FirstPhaseExplosion");
        assert(mTweener);

        mTweener->ChangeState("s_FirstPhaseExplosion");
    }

    void BlackHoleMissileActor::TriggerLifecycle_SecondPhaseExplosion()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_SecondPhaseExplosion");
        assert(mTweener);

        mTweener->ChangeState("s_SecondPhaseExplosion");
    }

    void BlackHoleMissileActor::TriggerSpawn(const glm::vec3 &position)
    {
        LogInfo("BlackHoleMissileActor::TriggerSpawn => GoID: ", this->GetObjectId());
        mActivityState = eMissileActivityState::ACTIVE;
        SetIsEnabled(true);
        mCombatActivePhaseActor->GetMovementComponent()->Teleport(position);

        TriggerLifecycle_FirstPhaseActiveCombat();
    }

    void BlackHoleMissileActor::TriggerDisable()
    {
        LogInfo("BlackHoleMissileActor::TriggerDisable => GoID: ", this->GetObjectId());
        mActivityState = eMissileActivityState::IDLE;
        mTweener->InitRootState();
    }

    void BlackHoleMissileActor::TriggerExplosion()
    {
        const auto c_soundList = GetComponentsByType<SoundComponent>();
        assert(c_soundList.size());
        c_soundList.back()->PlayBuffer("explosion");
        TriggerLifecycle_FirstPhaseExplosion();
    }

    bool BlackHoleMissileActor::IsInsideLevel(const BoundingBox &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(),
                                           boundingBox.GetMax(),
                                           mCombatActivePhaseActor->GetRootComponent()->GetTranslation());
    }
}