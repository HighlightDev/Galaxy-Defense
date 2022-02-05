#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"

namespace Game
{

   MovementComponent::MovementComponent(const std::string &gameObjectName)
       : Component(gameObjectName), mSpeed(1.0f)
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

}
