#include "NoPhysicsMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

namespace EngineCore
{

   NoPhysicsMovementComponent::NoPhysicsMovementComponent(const std::shared_ptr<MovementComponentData> &movementComponentData)
       : MovementComponent(movementComponentData),
         m_actorRootComponent()
   {
      mCurrentSpeed = 0.01f;
   }

   void NoPhysicsMovementComponent::OnPostOwnerInitialized()
   {
      MovementComponent::OnPostOwnerInitialized();

      if (const auto &spOwner = GetOwner().lock())
      {
         assert(spOwner->GetRootComponent());
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

   void NoPhysicsMovementComponent::Tick(const float deltaTime)
   {
      MovementComponent::Tick(deltaTime);
   }

   void NoPhysicsMovementComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
   }

   void NoPhysicsMovementComponent::Move(const float deltaTime)
   {
      if (const auto &actorRootComponentSp = m_actorRootComponent.lock())
      {
         actorRootComponentSp->AddTranslation(GetVelocity() * deltaTime);
      }
   }

   void NoPhysicsMovementComponent::Move(const glm::vec3 &direction, const float deltaTime)
   {
      if (const auto &actorRootComponentSp = m_actorRootComponent.lock())
      {
         actorRootComponentSp->AddTranslation(direction * deltaTime);
      }
   }

   void NoPhysicsMovementComponent::Jump()
   {
   }

   void NoPhysicsMovementComponent::Teleport(const glm::vec3 &teleportPosition)
   {
      if (const auto &actorRootComponentSp = m_actorRootComponent.lock())
      {
         actorRootComponentSp->SetTranslation(teleportPosition);
      }
   }

   glm::vec3 NoPhysicsMovementComponent::GetVelocity() const
   {
      return mDirection * mCurrentSpeed;
   }
}
