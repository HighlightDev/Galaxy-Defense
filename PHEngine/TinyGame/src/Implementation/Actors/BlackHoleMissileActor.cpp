#include "BlackHoleMissileActor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
    BlackHoleMissileActor::BlackHoleMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : SpaceEntityActor(gameObjectName, rootComponent)
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

    void BlackHoleMissileActor::OnTweenStateChanged(const std::string& stateName)
    {
        LogInfo("BlackHoleMissileActor::OnTweenStateChanged => New state: ", stateName);

        if ("s_FirstPhasePreload" == stateName)
        {
            TriggerLifecycle_FirstPhaseActiveCombat();
        }
        else if ("s_FirstPhaseActiveCombat" == stateName)
        {
            TriggerLifecycle_FirstPhaseExplosion();
        }
        else if ("s_FirstPhaseExplosion" == stateName)
        {
            TriggerLifecycle_SecondPhasePreload();
        }
        else if ("s_SecondPhasePreload" == stateName)
        {
            TriggerLifecycle_SecondPhaseExplosion();
        }
        else if ("s_SecondPhaseExplosion" == stateName)
        {
            LogInfo("BlackHoleMissileActor::OnTweenStateChanged => All stages are completed");
        }
        else {
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

    void BlackHoleMissileActor::TriggerLifecycle_SecondPhasePreload()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_SecondPhasePreload");
        assert(mTweener);

        mTweener->ChangeState("s_SecondPhasePreload");
    }

    void BlackHoleMissileActor::TriggerLifecycle_SecondPhaseExplosion()
    {
        LogInfo("BlackHoleMissileActor::TriggerLifecycle_SecondPhaseExplosion");
        assert(mTweener);

        mTweener->ChangeState("s_SecondPhaseExplosion");
    }
}