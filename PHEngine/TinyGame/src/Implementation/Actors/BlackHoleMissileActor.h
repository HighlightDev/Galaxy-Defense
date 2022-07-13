#pragma once

#include "MissileActor.h"
#include "Core/GameCore/Tweener/Tweener.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class BlackHoleMissileActor
        : public MissileActor,
          public ITweenStateChangeNotifyable
    {
    protected:
        std::shared_ptr<Actor> mCombatActivePhaseActor;
        std::shared_ptr<Actor> mExplosionSecondPhaseActor;

    public:
        BlackHoleMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void AttachTweener(std::shared_ptr<Tweener> tweener) override;

        virtual void OnTweenStateChanged(const std::string &stateName) override;

        virtual void TriggerSpawn(const glm::vec3 &position) override;

        virtual void TriggerExplosion() override;

        virtual void TriggerDisable() override;

        virtual bool IsInsideLevel(const BoundingBox &boundingBox) const override;

        void AddCombatActivePhaseActor(const std::shared_ptr<Actor> &combatActivePhaseActor);

        void AddExplosionSecondPhaseActor(const std::shared_ptr<Actor> &explosionSecondPhaseActor);

    private:
        void InitTweenerSubscriptions();

        void TriggerLifecycle_FirstPhasePreload();

        void TriggerLifecycle_FirstPhaseActiveCombat();

        void TriggerLifecycle_FirstPhaseExplosion();

        void TriggerLifecycle_SecondPhaseExplosion();
    };
}