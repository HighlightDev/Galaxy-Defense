#pragma once

#include "Implementation/Actors/MissileActor.h"

#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
class SceneComponent;
class ForceBarrierBeamComponent;
class BillboardComponent;
} // namespace EngineCore

namespace Game {
class CombatActorsPoolHandler;
class SpaceStationActor;
class BarrierActor;
class MissileExplosionVisitorBase;

// Force Barrier: a pooled ray weapon (like the repair beam) that corkscrews a golden spiral beam onto the nearest
// barrier and deploys a shield billboard at the impact point. Targeting + collision registration mirror the repair
// beam; the shield + spiral are self-animating. Future passes add the corner flares, reflection and dissipation phases.
class ForceBarrierRayActor : public MissileActor {
    std::shared_ptr<EngineCore::ForceBarrierBeamComponent> mBeamComponent;

    // Camera-facing shield billboard parked at the target (barrier) end.
    std::shared_ptr<EngineCore::BillboardComponent> mShield;

    std::weak_ptr<SpaceStationActor> mSpawnerWp;

    glm::vec3 mBeginWorldSpacePosition{0.0f};
    glm::vec3 mEndWorldSpacePosition{0.0f};

    bool mIsCollided{false};
    std::weak_ptr<BarrierActor> mCollidedBarrierWp;

public:
    ForceBarrierRayActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    bool IsInsideLevel(const BoundingBox3D& boundingBox) const override;

    void TriggerSpawn(
        const glm::vec3& position,
        const glm::vec3& direction,
        const float yawDegrees,
        const eDamageDealerType ownerType,
        const std::shared_ptr<SpaceStationActor>& spawnerActor) override;

    void TriggerDisabled() override;

    void SetBeamComponent(const std::shared_ptr<EngineCore::ForceBarrierBeamComponent>& beamComponent);

    void SetShield(const std::shared_ptr<EngineCore::BillboardComponent>& shield);

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    // The space station that launched this ray (the one whose "ray active" flag is toggled while the beam is connected).
    std::weak_ptr<SpaceStationActor> GetActorWhoSpawnedMeWp() const;

private:
    // Finds the nearest active barrier pillar in range, points the beam at it and registers the collision (else
    // disables the beam).
    void UpdateTargeting();

    void SendShootRayCollisionEvent(const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType);

    void SetEndpoints(const glm::vec3& beginWorldSpacePosition, const glm::vec3& endWorldSpacePosition);
};
} // namespace Game
