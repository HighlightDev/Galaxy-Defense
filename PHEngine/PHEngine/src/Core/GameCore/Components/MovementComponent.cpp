#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"

namespace Game
{

   MovementComponent::MovementComponent(const std::string &gameObjectName, std::weak_ptr<Actor> owner)
       : Component(gameObjectName), mSpeed(1.0f), mOwner(owner)
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

   void MovementComponent::PostLevelInit()
   {
      mOwner = GetOwner();
   }
}
