#include "MovementComponent.h"

#include "Core/GameCore/ICamera.h"

namespace Game
{

   MovementComponent::MovementComponent(ICamera* const mainCamera, glm::vec3 launchVelocity)
      : Component()
      , m_mainCamera(mainCamera)
      , Velocity(launchVelocity)
      , Speed(0.005f)
   {
   }

   MovementComponent::~MovementComponent()
   {
   }

   ICamera* const MovementComponent::GetCamera() const
   {
      return m_mainCamera;
   }

   uint64_t MovementComponent::GetComponentType() const
   {
      return MOVEMENT_COMPONENT;
   }

   void MovementComponent::Tick(const float deltaTime)
   {
      Velocity = m_mainCamera->GetEyeSpaceForwardVector();
   }

   glm::vec3 MovementComponent::GetMoveOffset() const
   {
      return Velocity * Speed;
   }
}
