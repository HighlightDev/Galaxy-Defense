#pragma once

#include "SpaceshipActor.h"
#include "Core/GameCore/Tweener/Tweener.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class WeakSpaceshipActor
        : public SpaceshipActor,
          public ITweenStateChangeNotifyable
    {

        std::shared_ptr<Tweener> mWeakSpaceshipTweener;

    public:
        WeakSpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void AttachTweener(std::shared_ptr<Tweener> tweener) override;

        virtual void OnTweenStateChanged(const std::string &stateName) override;

    private:
        void InitTweenerSubscriptions();
    };
}