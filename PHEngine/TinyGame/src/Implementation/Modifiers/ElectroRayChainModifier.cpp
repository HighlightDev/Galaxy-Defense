#include "ElectroRayChainModifier.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Pools/ElectroRayChainActorPool.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Game
{
    ElectroRayChainModifier::ElectroRayChainModifier(const std::weak_ptr<Actor> &chainDst, const std::weak_ptr<Actor> &chainSrc)
        : mChainDst(chainDst),
          mChainSrc(chainSrc),
          mElectroRayChainActor()
    {
    }

    void ElectroRayChainModifier::Initialize(const std::shared_ptr<::Game::ElectroRayChainActorPool> &mElectroRayChainActorPool)
    {
        mElectroRayChainActor = mElectroRayChainActorPool->GetFreeActor();
        assert(mElectroRayChainActor);
        mElectroRayChainActor->SetStartLineSpaceship(mChainSrc);
        mElectroRayChainActor->SetEndLineSpaceship(mChainDst);
        mElectroRayChainActor->TriggerSpawn({});

        mDisposeTimer.SetIntervalMs(2500);
        mDisposeTimer.SetIsRepeat(false);
        mDisposeTimer.SetIsPausable(true);
        mDisposeTimer.SetCallback(std::bind(&ElectroRayChainModifier::OnDisposeTimerTimeout, this));
        mDisposeTimer.StartTimer();
    }

    eModifierType ElectroRayChainModifier::GetModifierType() const
    {
        return eModifierType::ElectroChain;
    }

    uint64_t ElectroRayChainModifier::CreatorObjectId() const
    {
        const auto &chainDstSp = mChainDst.lock();
        assert(chainDstSp);
        return chainDstSp->GetObjectId();
    }

    void ElectroRayChainModifier::Tick(const float deltaTime)
    {
    }

    void ElectroRayChainModifier::OnPreRemoved()
    {
    }

    bool ElectroRayChainModifier::IsExpired() const
    {
        return !mDisposeTimer.IsRunning();
    }

    void ElectroRayChainModifier::OnDisposeTimerTimeout()
    {
        LogInfo("ElectroRayChainModifier::OnDisposeTimerTimeout => Disable actor:", mElectroRayChainActor->GetName());
        mElectroRayChainActor->TriggerDisabled();
    }
}