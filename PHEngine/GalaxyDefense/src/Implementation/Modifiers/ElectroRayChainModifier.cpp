#include "ElectroRayChainModifier.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

using namespace EngineCore;

namespace Game {
ElectroRayChainModifier::ElectroRayChainModifier(
    const std::pair<eGameObjectsType, const std::weak_ptr<Actor>>& chainDst,
    const std::pair<eGameObjectsType, const std::weak_ptr<Actor>>& chainSrc)
    : mChainDst(chainDst)
    , mChainSrc(chainSrc)
    , mElectroRayChainActor()
{
}

void ElectroRayChainModifier::Initialize(const std::shared_ptr<::Game::ElectroRayChainActor>& electroRayChainActor)
{
    mElectroRayChainActor = electroRayChainActor;
    ext_assert(mElectroRayChainActor, "ElectroRayChainModifier actor pointer is null");
    mElectroRayChainActor->SetStartLineSpaceship(mChainSrc.second);
    mElectroRayChainActor->SetEndLineSpaceship(mChainDst.second);
    mElectroRayChainActor->TriggerSpawn({}, {}, 0.0f, eDamageDealerType::MAIN_PLAYER, nullptr);
}

eModifierType ElectroRayChainModifier::GetModifierType() const
{
    return eModifierType::ElectroChain;
}

int32_t ElectroRayChainModifier::CreatorObjectId() const
{
    const auto& chainDstSp = mChainDst.second.lock();
    ext_assert(chainDstSp, "ElectroRayChainModifier chain destination pointer is null");
    return chainDstSp->GetObjectId();
}

void ElectroRayChainModifier::Tick(const float deltaTimeSec)
{
    if (mElectroRayChainActor->IsPendingDisable()) {
        mElectroRayChainActor->SetIsPendingDisable(false);
        mElectroRayChainActor->TriggerDisabled();
        mIsPendingRemoval = true;

        if (eGameObjectsType::SPACESHIP == mChainDst.first) {
            if (const auto& chainDstSp = mChainDst.second.lock()) {
                const auto& dstSpaceshipSp = std::static_pointer_cast<SpaceshipActor>(chainDstSp);
                const uint32_t dmg = static_cast<uint32_t>(Random::Float() * 5.0f) + 3;
                dstSpaceshipSp->TriggerDamageReceived(dmg, mElectroRayChainActor->GetDamageDealerType());
            }
        } else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == mChainDst.first) {
            if (const auto& chainDstSp = mChainDst.second.lock()) {
                const auto& dstSpaceobjectSp = std::static_pointer_cast<SpaceObjectActor>(chainDstSp);
                dstSpaceobjectSp->TriggerDisabled();
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
} // namespace Game