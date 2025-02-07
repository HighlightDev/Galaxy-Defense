#pragma once
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "MovementComponent.h"

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

using namespace Event;
using namespace EnginePhysics;

namespace EngineCore {
struct MovementComponentData;

class HumanoidPhysicsMovementComponent : public MovementComponent, public CameraTransformChangedGameThreadEvent {

    float mCameraYaw;
    float mCameraPitch;

    std::string mCameraName;

    bool bIsCameraRotationDirty = false;

    std::weak_ptr<CharacterPhysicsComponent> m_playerPhysicsComponent;

public:
    HumanoidPhysicsMovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData);

    ~HumanoidPhysicsMovementComponent() override;

    void Initialize() override;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTime) override;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    void ProcessEvent(
        const CameraTransformChangedGameThreadEvent* sender,
        const typename CameraTransformChangedGameThreadEvent::EventData_t& data) override;

    void Move(const float deltaTime) override;

    void Move(const glm::vec3& direction, const float deltaTime) override;

    void Jump() override;

    void Teleport(const glm::vec3& teleportPosition) override;

    void OnSceneOwnerInitialized() override;

protected:
    glm::vec3 GetVelocity() const;

    glm::mat3 GetCameraYawRotationMatrix() const;

    glm::vec3 GetCameraPitchYawRoll() const;
};

} // namespace EngineCore
