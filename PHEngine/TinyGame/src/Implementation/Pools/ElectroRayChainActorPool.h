#pragma once

#include <memory>
#include <vector>

namespace EngineCore {
class Scene;
}

namespace Game {
class ElectroRayChainActor;

class ElectroRayChainActorPool {
    std::vector<std::shared_ptr<ElectroRayChainActor>> mPool;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

public:
    explicit ElectroRayChainActorPool(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    std::shared_ptr<ElectroRayChainActor> GetFreeActor();

private:
    std::shared_ptr<ElectroRayChainActor> Spawn();
};
} // namespace Game