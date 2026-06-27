#pragma once

#include "Implementation/Actors/MissileActor.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class SceneComponent;
class RepairBeamComponent;
class StaticMeshComponent;
class InstancedStaticMeshComponent;
class BillboardComponent;
} // namespace EngineCore

namespace Game {
class CombatActorsPoolHandler;
class SpaceStationActor;
class BarrierActor;
class MissileExplosionVisitorBase;

class RepairBeamActor : public MissileActor {
    std::shared_ptr<EngineCore::RepairBeamComponent> mBeamComponent;

    std::vector<std::shared_ptr<EngineCore::InstancedStaticMeshComponent>> mNanobots;

    std::shared_ptr<EngineCore::BillboardComponent> mHealingCross;
    std::shared_ptr<EngineCore::BillboardComponent> mBurstRings;

    std::shared_ptr<EngineCore::StaticMeshComponent> mPulseBall;
    bool mPulseBallVisible{true};

    std::weak_ptr<SpaceStationActor> mSpawnerWp;

    glm::vec3 mBeginWorldSpacePosition{0.0f};
    glm::vec3 mEndWorldSpacePosition{0.0f};

    // Continuous clock driving the nanobots' travel + wave phase + pulse loop (real time, tracks the proxy's frames).
    float mElapsedSec{0.0f};

    bool mIsCollided{false};
    std::weak_ptr<BarrierActor> mCollidedBarrierWp;

    glm::vec3 mBeamLineBegin;
    glm::vec3 mBeamLineEnd;

public:
    // Pulse timing (seconds), shared with the burst material so the rings fire when the ball lands.
    static constexpr float c_pulseFlightDurationSec = 0.4f;
    static constexpr float c_pulseCyclePeriodSec = 1.6f;
    static constexpr float c_pulseRingDelaySec = 0.08f;
    static constexpr float c_pulseRingLifeSec = 0.4f;

    RepairBeamActor(
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

    void SetBeamComponent(const std::shared_ptr<EngineCore::RepairBeamComponent>& beamComponent);

    void SetNanobots(const std::vector<std::shared_ptr<EngineCore::InstancedStaticMeshComponent>>& nanobots);

    void SetHealingCross(const std::shared_ptr<EngineCore::BillboardComponent>& healingCross);

    void SetBurstRings(const std::shared_ptr<EngineCore::BillboardComponent>& burstRings);

    void SetPulseBall(const std::shared_ptr<EngineCore::StaticMeshComponent>& pulseBall);

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    // The space station that launched this beam (the one whose "ray active" flag is toggled while the beam is connected).
    std::weak_ptr<SpaceStationActor> GetActorWhoSpawnedMeWp() const;

private:
    // Casts the beam from the tower (RayCastWithFilterAdapter), locks onto the first barrier hit and registers the
    // collision via ShootRayCollisionEvent (like the electro ray); disables the beam when there is no target.
    void UpdateTargeting();

    void SendShootRayCollisionEvent(const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType);

    // Places the beam + the target-end billboards for the given endpoints.
    void SetEndpoints(const glm::vec3& beginWorldSpacePosition, const glm::vec3& endWorldSpacePosition);

    // World-space point on the rippling beam at parameter t in [0,1] (uses the beam's wave + basis).
    glm::vec3 GetWavePoint(const float t) const;

    void UpdateNanobots();

    void UpdatePulse();
};
} // namespace Game
