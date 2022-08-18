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

        virtual void PostLevelInit() override;

        virtual void TriggerDamageReceived(const size_t damage) override;

        virtual void TriggerSpawn(const glm::vec3 &position) override;

        virtual void TriggerExplosion() override;

        virtual void TriggerDisabled() override;

    private:
        virtual void OnTweenStateChanged(const std::string &stateName) override;

        void InitTweenerSubscriptions();
    };
}