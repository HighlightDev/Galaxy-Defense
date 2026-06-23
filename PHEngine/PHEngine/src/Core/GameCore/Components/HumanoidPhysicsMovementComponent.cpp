#include "HumanoidPhysicsMovementComponent.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

HumanoidPhysicsMovementComponent::HumanoidPhysicsMovementComponent(
    const std::shared_ptr<MovementComponentData>& movementComponentData)
    : MovementComponent(movementComponentData)
    , CameraTransformChangedGameThreadEvent()
    , mCameraName("")
    , m_playerPhysicsComponent()
{
    const auto& charMoveCompData = std::static_pointer_cast<HumanoidMovementComponentData>(movementComponentData);
    mCameraName = charMoveCompData->mCameraName;
}

void HumanoidPhysicsMovementComponent::OnSceneOwnerInitialized()
{
    MovementComponent::OnSceneOwnerInitialized();

    if (const auto& spOwner = GetOwner().lock()) {
        const auto& characterPhysicsComponent
            = std::dynamic_pointer_cast<CharacterPhysicsComponent>(spOwner->GetPhysicsComponent());
        ext_assert(characterPhysicsComponent, "HumanoidPhysicsMovementComponent requires CharacterPhysicsComponent");
        m_playerPhysicsComponent = characterPhysicsComponent;
    }
}

HumanoidPhysicsMovementComponent::~HumanoidPhysicsMovementComponent()
{
    CameraTransformChangedGameThreadEvent::GetInstance()->RemoveListener(CameraTransformChangedGameThreadEvent::GetInstanceId());
}

void HumanoidPhysicsMovementComponent::OnRegistered()
{
    MovementComponent::OnRegistered();

    CameraTransformChangedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<HumanoidPhysicsMovementComponent>(shared_from_this()));
}

eComponentType HumanoidPhysicsMovementComponent::GetComponentType() const
{
    return CHARACTER_MOVEMENT_COMPONENT;
}

void HumanoidPhysicsMovementComponent::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (bIsCameraRotationDirty) {
        if (const auto& spOwner = GetOwner().lock()) {
            spOwner->GetRootComponent()->SetAdditionalRotation(GetCameraPitchYawRoll());
        }

        bIsCameraRotationDirty = false;
    }
}

void HumanoidPhysicsMovementComponent::Teleport(const glm::vec3& teleportPosition)
{
}

void HumanoidPhysicsMovementComponent::ProcessEvent(
    const CameraTransformChangedGameThreadEvent* sender, const CameraTransformChangedGameThreadEvent::EventData_t& data)
{
    auto& cameraPtr = std::get<0>(data);

    if (cameraPtr->GetCameraName() == mCameraName) {
        mDirection = cameraPtr->GetEyeSpaceForwardVector();
        mCameraYaw = cameraPtr->GetRotationYaw();

        bIsCameraRotationDirty = true;
    }
}

void HumanoidPhysicsMovementComponent::Move(const float deltaTimeSec)
{
    if (const auto& playerPhysComponentSp = m_playerPhysicsComponent.lock()) {
        playerPhysComponentSp->SetWalkVelocity(GetVelocity() * deltaTimeSec);
    }
}

void HumanoidPhysicsMovementComponent::Move(const glm::vec3& direction, const float deltaTimeSec)
{
    if (const auto& playerPhysComponentSp = m_playerPhysicsComponent.lock()) {
        playerPhysComponentSp->SetWalkVelocity(direction * deltaTimeSec * mCurrentSpeed);
    }
}

void HumanoidPhysicsMovementComponent::Jump()
{
    if (const auto& playerPhysComponentSp = m_playerPhysicsComponent.lock()) {
        playerPhysComponentSp->SetJumpVelocity();
    }
}

glm::vec3 HumanoidPhysicsMovementComponent::GetVelocity() const
{
    return mDirection * mCurrentSpeed;
}

glm::mat3 HumanoidPhysicsMovementComponent::GetCameraYawRotationMatrix() const
{
    return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
}

glm::vec3 HumanoidPhysicsMovementComponent::GetCameraPitchYawRoll() const
{
    return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
}
} // namespace EngineCore
