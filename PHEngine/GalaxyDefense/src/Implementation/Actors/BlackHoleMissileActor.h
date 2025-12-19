#pragma once

#include "Core/GameCore/Tweener/Tweener.h"
#include "MissileActor.h"

#include <memory>

using namespace EngineCore;

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;
class SpaceStationActor;

class BlackHoleMissileActor : public MissileActor, public ITweenStateChangeNotifyable {
protected:
    std::shared_ptr<Actor> mCombatActivePhaseActor;

    std::shared_ptr<MissileActor> mExplosionSecondPhaseActor;

    std::shared_ptr<Tweener> mBlackMissileTweener;

    std::shared_ptr<SpaceStationActor> mSpawnerActor;

public:
    BlackHoleMissileActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& poolHandler);

    void AttachTweener(std::shared_ptr<Tweener> tweener) override;

    void OnTweenStateChanged(const std::string& stateName) override;

    void TriggerSpawn(
        const glm::vec3& position,
        const glm::vec3& direction,
        const float yawDegrees,
        const eDamageDealerType ownerType,
        const std::shared_ptr<SpaceStationActor>& spawnerActor) override;

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