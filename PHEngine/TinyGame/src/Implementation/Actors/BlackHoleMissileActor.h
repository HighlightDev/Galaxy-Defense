#pragma once

#include "SpaceEntityActor.h"
#include "Core/GameCore/Tweener/Tweener.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class BlackHoleMissileActor 
    : public SpaceEntityActor
    , public ITweenStateChangeNotifyable
    {
        public:

        BlackHoleMissileActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

        virtual void AttachTweener(std::shared_ptr<Tweener> tweener) override;

        virtual void OnTweenStateChanged(const std::string& stateName) override;

        void TriggerLifecycle_FirstPhasePreload();

        void TriggerLifecycle_FirstPhaseActiveCombat();

        void TriggerLifecycle_FirstPhaseExplosion();

        void TriggerLifecycle_SecondPhasePreload();

        void TriggerLifecycle_SecondPhaseExplosion();

        private:

        void InitTweenerSubscriptions();
    };
}