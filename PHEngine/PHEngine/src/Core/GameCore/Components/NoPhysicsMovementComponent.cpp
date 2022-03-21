#include "NoPhysicsMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

namespace EngineCore
{

   NoPhysicsMovementComponent::NoPhysicsMovementComponent(const MovementComponentData& movementComponentData)
       : MovementComponent(movementComponentData), m_actorRootComponent()
   {
      mSpeed = 0.01f;
   }

   void NoPhysicsMovementComponent::PostLevelInit()
   {
      MovementComponent::PostLevelInit();
       
      if (const auto &spOwner = GetOwner().lock())
      {
         m_actorRootComponent = spOwner->GetRootComponent();
         assert(m_actorRootComponent);
      }
   }

   NoPhysicsMovementComponent::~NoPhysicsMovementComponent()
   {
   }

   ComponentType NoPhysicsMovementComponent::GetComponentType() const
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

   void NoPhysicsMovementComponent::Move()
   {
      m_actorRootComponent->AddTranslation(GetVelocity());
   }

   void NoPhysicsMovementComponent::Jump()
   {
   }

   void NoPhysicsMovementComponent::Teleport(const glm::vec3& teleportPosition)
   {
      m_actorRootComponent->SetTranslation(teleportPosition);
   }

   glm::vec3 NoPhysicsMovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }
}
