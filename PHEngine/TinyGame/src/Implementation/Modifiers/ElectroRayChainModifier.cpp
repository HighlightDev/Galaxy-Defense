#include "ElectroRayChainModifier.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Factories/ElectroRayChainFactory.h"

using namespace EngineCore;

namespace Game
{
    ElectroRayChainModifier::ElectroRayChainModifier(const std::weak_ptr<Actor> &chainDst, const std::weak_ptr<Actor> &chainSrc)
        : mChainDst(chainDst),
          mChainSrc(chainSrc),
          mElectroRayChainActor()
    {
    }

    void ElectroRayChainModifier::Initialize()
    {
        mElectroRayChainActor = CreateElectroRayChainActor();
    }

    std::shared_ptr<ElectroRayChainActor> ElectroRayChainModifier::CreateElectroRayChainActor() const
    {
        std::shared_ptr<ElectroRayChainActor> chainActor;
        if (const auto chainDstSp = mChainDst.lock())
        {
            if (const auto &sceneSp = chainDstSp->GetSceneOwner().lock())
            {
                ElectroRayChainFactory factory;
                chainActor = std::static_pointer_cast<ElectroRayChainActor>(factory.CreateMissileExplosionChain(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1)));
                chainActor->SetStartLineSpaceship(mChainSrc);
                chainActor->SetEndLineSpaceship(mChainDst);
            }
        }
        return chainActor;
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
        //todo: Delete actor with all components
    }

    bool ElectroRayChainModifier::IsExpired() const
    {
        return false;
    }
}