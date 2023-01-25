#include "ElectroRayChainModifier.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Pools/ElectroRayChainActorPool.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Game
{
    ElectroRayChainModifier::ElectroRayChainModifier(const std::pair<eGameObjectsType, const std::weak_ptr<Actor>> &chainDst,
                                                     const std::pair<eGameObjectsType, const std::weak_ptr<Actor>> &chainSrc)
        : mChainDst(chainDst),
          mChainSrc(chainSrc),
          mElectroRayChainActor()
    {
    }

    void ElectroRayChainModifier::Initialize(const std::shared_ptr<::Game::ElectroRayChainActorPool> &mElectroRayChainActorPool)
    {
        mElectroRayChainActor = mElectroRayChainActorPool->GetFreeActor();
        assert(mElectroRayChainActor);
        mElectroRayChainActor->SetStartLineSpaceship(mChainSrc.second);
        mElectroRayChainActor->SetEndLineSpaceship(mChainDst.second);
        mElectroRayChainActor->TriggerSpawn({});
    }

    eModifierType ElectroRayChainModifier::GetModifierType() const
    {
        return eModifierType::ElectroChain;
    }

    uint64_t ElectroRayChainModifier::CreatorObjectId() const
    {
        const auto &chainDstSp = mChainDst.second.lock();
        assert(chainDstSp);
        return chainDstSp->GetObjectId();
    }

    void ElectroRayChainModifier::Tick(const float deltaTime)
    {
        if (mElectroRayChainActor->IsPendingDisable())
        {
            mElectroRayChainActor->SetIsPendingDisable(false);
            mElectroRayChainActor->TriggerDisabled();
            mIsPendingRemoval = true;

            if (eGameObjectsType::SPACESHIP == mChainDst.first)
            {
                if (const auto &chainDstSp = mChainDst.second.lock())
                {
                    const auto &dstSpaceshipSp = std::static_pointer_cast<SpaceshipActor>(chainDstSp);
                    const size_t dmg = std::max((size_t)(Random::Float() * 2.0f), static_cast<size_t>(1));
                    dstSpaceshipSp->TriggerDamageReceived(dmg);
                }
            }
        }
    }

    void ElectroRayChainModifier::OnPreRemoved()
    {
        mElectroRayChainActor->TriggerDisabled();
    }

    bool ElectroRayChainModifier::IsExpired() const
    {
        return mIsPendingRemoval;
    }
}