#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/Actors/MissileActor.h"

#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
class DynamicBeamComponent;
class SpaceStationActor;
class Actor;
} // namespace EngineCore

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;

class ElectroRayActor : public MissileActor {
    std::shared_ptr<::EngineCore::DynamicBeamComponent> mLineComponent;
    std::weak_ptr<SpaceStationActor> mSpaceshipWhoSpawnedMeWp;

    glm::vec3 mElectroLineDirection;
    glm::vec3 mElectroLineBegin;
    glm::vec3 mElectroLineEnd;

    float mElectroLineOriginSpeed;
    float mElectroLineDestinationSpeed;

    std::shared_ptr<GameThreadTimer> mElectroLineOriginStartMovementDelayTimer;
    bool bLineOriginStartMovement;

    std::weak_ptr<::EngineCore::Actor> mCollidedSpaceship;
    bool bElectroLineCollided;

    std::shared_ptr<EngineObjectProperty<float>> mOpacity;

public:
    ElectroRayActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    bool IsInsideLevel(const BoundingBox3D& boundingBox) const override;

    void OnSceneOwnerInitialized() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void TriggerSpawn(
        const glm::vec3& position,
        const glm::vec3& direction,
        const float yawDegrees,
        const eDamageDealerType ownerType,
        const std::shared_ptr<SpaceStationActor>& spawnerActor) override;

    void TriggerExplosion() override;

    void TriggerExplosionFinished() override;

    void TriggerDisabled() override;

    void SetLineComponent(const std::shared_ptr<::EngineCore::DynamicBeamComponent>& lineComponent);

    void SetElectroLineOriginSpeed(const float speed);

    void SetElectroLineDestinationSpeed(const float speed);

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

private:
    void Initialize();

    void OnElectroLineOriginStartMovementDelayTimerTimeout();

    void OnElectroLineFadeoutTimerTimeout();

    void DropState();
};
} // namespace Game