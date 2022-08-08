#include "WeakSpaceshipActor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game
{
    WeakSpaceshipActor::WeakSpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : SpaceshipActor(gameObjectName, rootComponent), ITweenStateChangeNotifyable(),
          mWeakSpaceshipTweener()
    {
    }

    void WeakSpaceshipActor::AttachTweener(std::shared_ptr<Tweener> tweener)
    {
        assert(tweener);
        LogInfo("WeakSpaceshipActor::AttachTweener => Path to tweener", tweener->GetRelPathTweener());

        Actor::AttachTweener(tweener);
        mWeakSpaceshipTweener = tweener;
        InitTweenerSubscriptions();
    }

    void WeakSpaceshipActor::InitTweenerSubscriptions()
    {
        mWeakSpaceshipTweener->SubscribeOnStateChange(this);
    }

    void WeakSpaceshipActor::OnTweenStateChanged(const std::string &stateName)
    {
        LogInfo("WeakSpaceshipActor::OnTweenStateChanged => New state: ", stateName);
    }
}