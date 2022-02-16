#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

namespace Game
{

   MovementComponent::MovementComponent(const MovementComponentData& movementComponentData)
       : Component(movementComponentData.GameObjectName), mSpeed(1.0f), mDirection(movementComponentData.m_launchDirection)
   {
   }

   MovementComponent::~MovementComponent()
   {
   }

   ComponentType MovementComponent::GetComponentType() const
   {
      return MOVEMENT_COMPONENT;
   }

   float MovementComponent::GetSpeed() const
   {
      return mSpeed;
   }

   void MovementComponent::SetSpeed(const float speed)
   {
      mSpeed = speed;
   }

   void MovementComponent::SetDirection(const glm::vec3 &direction)
   {
      mDirection = direction;
   }

   glm::vec3 MovementComponent::GetDirection() const
   {
      return mDirection;
   }

}
