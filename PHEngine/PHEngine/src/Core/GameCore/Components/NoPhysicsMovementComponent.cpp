#include "NoPhysicsMovementComponent.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

NoPhysicsMovementComponent::NoPhysicsMovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData)
    : MovementComponent(movementComponentData)
    , m_actorRootComponent()
{
    mCurrentSpeed = 0.01f;
}

void NoPhysicsMovementComponent::OnPostOwnerInitialized()
{
    MovementComponent::OnPostOwnerInitialized();

    if (const auto& spOwner = GetOwner().lock()) {
        ext_assert(
            spOwner->GetRootComponent(),
            "Owner Actor has no RootComponent in NoPhysicsMovementComponent::OnPostOwnerInitialized");
        m_actorRootComponent = spOwner->GetRootComponent();
    }
}

NoPhysicsMovementComponent::~NoPhysicsMovementComponent()
{
}

eComponentType NoPhysicsMovementComponent::GetComponentType() const
{
    return MOVEMENT_COMPONENT;
}

void NoPhysicsMovementComponent::Tick(const float deltaTimeSec, const float playSpeed)
{
    MovementComponent::Tick(deltaTimeSec, playSpeed);
}

void NoPhysicsMovementComponent::Move(const float deltaTimeSec)
{
    if (const auto& actorRootComponentSp = m_actorRootComponent.lock()) {
        actorRootComponentSp->AddTranslation(GetVelocity() * deltaTimeSec);
    }
}

void NoPhysicsMovementComponent::Move(const glm::vec3& direction, const float deltaTimeSec)
{
    if (const auto& actorRootComponentSp = m_actorRootComponent.lock()) {
        actorRootComponentSp->AddTranslation(direction * deltaTimeSec);
    }
}

void NoPhysicsMovementComponent::Jump()
{
}

void NoPhysicsMovementComponent::Teleport(const glm::vec3& teleportPosition)
{
    if (const auto& actorRootComponentSp = m_actorRootComponent.lock()) {
        actorRootComponentSp->SetTranslation(teleportPosition);
    }
}

glm::vec3 NoPhysicsMovementComponent::GetVelocity() const
{
    return mDirection * mCurrentSpeed;
}
} // namespace EngineCore
