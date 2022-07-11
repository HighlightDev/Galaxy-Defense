#pragma once

#include "SpaceEntityActor.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/BoundingBox.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    enum class eMissileActivityState
    {
        IDLE,
        ACTIVE,
    };

    class BlackHoleMissileActor
        : public SpaceEntityActor,
          public ITweenStateChangeNotifyable
    {
    protected:
        std::shared_ptr<Actor> mCombatActivePhaseActor;

        eMissileActivityState mActivityState {eMissileActivityState::IDLE};

    public:
        BlackHoleMissileActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void AttachTweener(std::shared_ptr<Tweener> tweener) override;

        virtual void OnTweenStateChanged(const std::string &stateName) override;

        void AddCombatActivePhaseActor(const std::shared_ptr<Actor> &combatActivePhaseActor);

        void TriggerSpawn(const glm::vec3 &position);

        void TriggerActivePhaseExplosion();

        void TriggerDisable();

        bool IsInsideLevel(const BoundingBox &boundingBox) const;

        eMissileActivityState GetMissileActivityState() const;

    private:
        void InitTweenerSubscriptions();

        void TriggerLifecycle_FirstPhasePreload();

        void TriggerLifecycle_FirstPhaseActiveCombat();

        void TriggerLifecycle_FirstPhaseExplosion();

        void TriggerLifecycle_SecondPhasePreload();

        void TriggerLifecycle_SecondPhaseExplosion();
    };
}