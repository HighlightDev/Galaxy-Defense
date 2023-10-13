#include "ElectroRayChainActorPool.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Factories/ElectroRayChainFactory.h"
#include "Core/GameCore/Scene.h"

#include <algorithm>

using namespace EngineCore;

namespace Game
{
    ElectroRayChainActorPool::ElectroRayChainActorPool(const std::weak_ptr<::EngineCore::Scene> &sceneWp)
        : mPool(),
          mSceneWp(sceneWp)
    {
    }

    std::shared_ptr<ElectroRayChainActor> ElectroRayChainActorPool::GetFreeActor()
    {
        const auto freeChainActorIt = std::find_if(mPool.cbegin(), mPool.cend(), [](const auto &actorInPool)
                                                   { return eMissileActivityState::IDLE == actorInPool->GetMissileActivityState(); });
        if (freeChainActorIt != mPool.cend())
        {
            return (*freeChainActorIt);
        }

        return Spawn();
    }

    std::shared_ptr<ElectroRayChainActor> ElectroRayChainActorPool::Spawn()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            ElectroRayChainFactory factory;
            const auto &spawnedActor = std::static_pointer_cast<ElectroRayChainActor>(factory.CreateMissile(sceneSp, nullptr, glm::vec3(), glm::vec3(), glm::vec3(1)));
            mPool.emplace_back(spawnedActor);
            spawnedActor->TriggerDisabled();
            return spawnedActor;
        }
        return nullptr;
    }
}