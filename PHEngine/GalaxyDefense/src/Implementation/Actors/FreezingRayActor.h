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
class RuntimeGeneratedLineComponent;
class Actor;
} // namespace EngineCore

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;

class FreezingRayActor : public MissileActor {
    std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> mLineComponent;

    std::weak_ptr<::EngineCore::Actor> mActorWhoSpawnedMeWp;

    glm::vec3 mFreezingLineBegin;
    glm::vec3 mFreezingLineEnd;

    float mFreezingRayHitRadius{0.0f};

    int32_t mLastCollidedActorId{-1};

    std::shared_ptr<EngineObjectProperty<float>> mOpacity;

    GameThreadTimer mSwitchTargetMinTimer;

public:
    FreezingRayActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    bool IsInsideLevel(const BoundingBox3D& boundingBox) const override;

    void Tick(const float deltaTime) override;

    void TriggerSpawn(
        const glm::vec3& position,
        const glm::vec3& direction,
        const float yawDegrees,
        const eDamageDealerType ownerType,
        const std::shared_ptr<::EngineCore::Actor>& spawnerActor) override;

    void TriggerExplosion() override;

    void TriggerExplosionFinished() override;

    void TriggerDisabled() override;

    void SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>& lineComponent);

    void SetFreezingRayHitRadius(const float radius);

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    std::weak_ptr<::EngineCore::Actor> GetActorWhoSpawnedMeWp() const;

private:
    void Initialize();

    void OnElectroLineOriginStartMovementDelayTimerTimeout();

    void OnElectroLineFadeoutTimerTimeout();

    void DropState();

    void OnCanSwitchTargetTimeout();

    std::vector<std::shared_ptr<PhysicsComponent>>
    CreateExcludedCollisionComponentsVector(const std::shared_ptr<PhysicsComponent>& collisionComponentOfWhoSpawnedMe) const;

    void SendShootRayCollisionEvent(const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType);
};
} // namespace Game