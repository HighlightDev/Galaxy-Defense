#include "SpaceEntityActor.h"

namespace Game
{
    SpaceEntityActor::SpaceEntityActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent),
          mLifecycleStateCallbacks()
    {
    }

    void SpaceEntityActor::SubscribeOnLifecycleStateChange(const std::string &lifecycleStateName,
                                                           std::function<void(const std::string &, const std::shared_ptr<Actor> &)> callback)
    {
        assert(!mLifecycleStateCallbacks.count(lifecycleStateName));
        mLifecycleStateCallbacks.emplace(lifecycleStateName, callback);
    }
}