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

        void PostLevelInit() override;

        void TriggerDamageReceived(const size_t damage, const eDamageDealerType damageDealerType) override;

        void TriggerSpawn(const glm::vec3 &position) override;

        void TriggerExplosion() override;

        void TriggerDisabled() override;

    private:
        void OnTweenStateChanged(const std::string &stateName) override;

        void InitTweenerSubscriptions();
    };
}