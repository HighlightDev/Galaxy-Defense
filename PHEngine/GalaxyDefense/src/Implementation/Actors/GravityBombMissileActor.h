#pragma once

#include "Core/GameCore/Tweener/Tweener.h"
#include "MissileActor.h"

#include <memory>

using namespace EngineCore;

namespace Graphics {
class IMaterial;
}

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;
class SpaceStationActor;

class GravityBombMissileActor : public MissileActor, public ITweenStateChangeNotifyable {
protected:
    std::shared_ptr<Actor> mCombatActivePhaseActor;

    std::shared_ptr<MissileActor> mExplosionSecondPhaseActor;

    std::shared_ptr<Tweener> mBlackMissileTweener;

    std::shared_ptr<SpaceStationActor> mSpawnerActor;

    // Identity-transform host actor that owns the gravity tether curves, and their shared material. World-space tether
    // geometry only renders correctly under an untransformed host (see GravityModifier).
    std::shared_ptr<Actor> mTetherHostActor;

    std::shared_ptr<Graphics::IMaterial> mTetherMaterial;

public:
    void SetTetherHostActor(const std::shared_ptr<Actor>& hostActor);

    const std::shared_ptr<Actor>& GetTetherHostActor() const;

    void SetTetherMaterial(const std::shared_ptr<Graphics::IMaterial>& material);

    const std::shared_ptr<Graphics::IMaterial>& GetTetherMaterial() const;

    GravityBombMissileActor(
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

    void TriggerLifecycle_FirstPhasePreload();

    void TriggerLifecycle_FirstPhaseActiveCombat();

    void TriggerLifecycle_Impact();

    void TriggerLifecycle_GravityCapture();

    void TriggerLifecycle_Collapse();

    void TriggerLifecycle_Fade();

    void TriggerLifecycle_GravityBombSuckIn();
};
} // namespace Game