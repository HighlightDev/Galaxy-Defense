#pragma once
#include "MovementComponent.h"
#include "SceneComponent.h"

namespace EngineCore
{
   struct MovementComponentData;
   
   class NoPhysicsMovementComponent
       : public MovementComponent
   {

      std::shared_ptr<SceneComponent> m_actorRootComponent;

   public:
      NoPhysicsMovementComponent(const MovementComponentData& movementComponentData);

      virtual ~NoPhysicsMovementComponent();

      virtual eComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      virtual void Move(const float deltaTime) override;

      virtual void Move(const glm::vec3& direction, const float deltaTime) override;

      virtual void Jump() override;

      virtual void Teleport(const glm::vec3& teleportPosition) override;

      virtual void PostLevelInit() override;

      glm::vec3 GetVelocity() const;
   };

}
