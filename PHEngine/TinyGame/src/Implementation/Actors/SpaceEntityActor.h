#pragma once

#include "Core/GameCore/Actor.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class SpaceEntityActor 
    : public Actor
    {
        protected:

        std::unordered_map<std::string/*lifecycle state name*/, std::function<void(const std::string&, const std::shared_ptr<Actor>&)>> mLifecycleStateCallbacks;

        public:

        SpaceEntityActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);
        
        void SubscribeOnLifecycleStateChange(const std::string& lifecycleStateName, std::function<void(const std::string&, const std::shared_ptr<Actor>&)> callback);
    };
}