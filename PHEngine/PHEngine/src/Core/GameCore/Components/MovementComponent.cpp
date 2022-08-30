#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

namespace EngineCore
{

   MovementComponent::MovementComponent(const MovementComponentData &movementComponentData)
       : Component(movementComponentData.GameObjectName),
         mReferenceSpeed(1.0f),
         mCurrentSpeed(1.0f),
         mDirection(movementComponentData.m_launchDirection)
   {
   }

   MovementComponent::~MovementComponent()
   {
   }

   eComponentType MovementComponent::GetComponentType() const
   {
      return MOVEMENT_COMPONENT;
   }

   void MovementComponent::SetCurrentSpeed(const float speed)
   {
      mCurrentSpeed = speed;
   }

   float MovementComponent::GetCurrentSpeed() const
   {
      return mCurrentSpeed;
   }

   void MovementComponent::SetReferenceSpeed(const float speed)
   {
      mReferenceSpeed = speed;
   }

   float MovementComponent::GetReferenceSpeed() const
   {
      return mReferenceSpeed;
   }

   void MovementComponent::SetCurrentSpeedToReferenceValue()
   {
      mCurrentSpeed = mReferenceSpeed;
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
