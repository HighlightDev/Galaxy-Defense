#pragma once

#include "Core/GameCore/Tweener/Tweener.h"
#include "MissileActor.h"

#include <memory>

using namespace EngineCore;

namespace EngineCore {
class PlasmaTrailComponent;
class CpuParticleSystemComponent;
} // namespace EngineCore

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;
class SpaceStationActor;

// Plasma bomb: a flying body that, on impact, opens a plasma breach and leaves a damaging plasma field/trail. Mirrors the
// two-actor (combat phase + field phase) lifecycle of GravityBombMissileActor, driven by a 7-state tweener.
class PlasmaBombMissileActor : public MissileActor, public ITweenStateChangeNotifyable {
protected:
    std::shared_ptr<Actor> mCombatActivePhaseActor;

    std::shared_ptr<MissileActor> mExplosionSecondPhaseActor;

    std::shared_ptr<Tweener> mPlasmaMissileTweener;

    std::shared_ptr<SpaceStationActor> mSpawnerActor;

    // The world-space flight trail, cached at creation so the lifecycle hooks (spawn/impact/disable) drive it directly
    // instead of scanning components every time. It hangs off this missile's identity root, so its recorded world-space
    // path renders without inheriting any parent transform.
    std::shared_ptr<EngineCore::PlasmaTrailComponent> mTrailComponent;

    // Scorch burst fired from the impact point when the plasma sears the target. Cached at creation; emitted on impact.
    std::shared_ptr<EngineCore::CpuParticleSystemComponent> mBurnParticleComponent;

public:
    void SetTrailComponent(const std::shared_ptr<EngineCore::PlasmaTrailComponent>& trailComponent);

    void SetBurnParticleComponent(const std::shared_ptr<EngineCore::CpuParticleSystemComponent>& burnParticleComponent);

    PlasmaBombMissileActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& poolHandler);

    void CleanUp() override;

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

    void TriggerLifecycle_FirstPhaseActiveCombat();

    void TriggerLifecycle_Impact();

    void TriggerLifecycle_Dissipate();
};
} // namespace Game
