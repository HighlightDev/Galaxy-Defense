#pragma once

#include "Core/GameCore/Tweener/Tweener.h"
#include "MissileActor.h"

#include <memory>

using namespace EngineCore;

namespace Game {
class MissileExplosionVisitorBase;

class BlackHoleMissileActor : public MissileActor, public ITweenStateChangeNotifyable {
protected:
    std::shared_ptr<Actor> mCombatActivePhaseActor;
    std::shared_ptr<MissileActor> mExplosionSecondPhaseActor;

    std::shared_ptr<Tweener> mBlackMissileTweener;

public:
    BlackHoleMissileActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void AttachTweener(std::shared_ptr<Tweener> tweener) override;

    void OnTweenStateChanged(const std::string& stateName) override;

    void TriggerSpawn(const glm::vec3& position, const eDamageDealerType ownerType) override;

    void TriggerExplosion() override;

    void TriggerExplosionFinished() override;

    void TriggerDisabled() override;

    bool IsInsideLevel(const BoundingBox3D& boundingBox) const override;

    void AddCombatActivePhaseActor(const std::shared_ptr<Actor>& combatActivePhaseActor);

    void AddExplosionSecondPhaseActor(const std::shared_ptr<MissileActor>& explosionSecondPhaseActor);

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    const std::shared_ptr<Actor>& GetCombatActivePhaseActor() const;

    const std::shared_ptr<MissileActor>& GetExplosionPhaseActor() const;

private:
    void InitTweenerSubscriptions();

    void TriggerLifecycle_FirstPhasePreload();

    void TriggerLifecycle_FirstPhaseActiveCombat();

    void TriggerLifecycle_FirstPhaseExplosion();

    void TriggerLifecycle_SecondPhaseExplosion();

    void TriggerLifecycle_BlackHoleSuckIn();
};
} // namespace Game