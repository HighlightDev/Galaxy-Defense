#pragma once
#include "MovementComponent.h"
#include "SceneComponent.h"

namespace EngineCore
{
   struct MovementComponentData;

   class NoPhysicsMovementComponent
       : public MovementComponent
   {

   protected:
      std::weak_ptr<SceneComponent> m_actorRootComponent;

   public:
      NoPhysicsMovementComponent(const std::shared_ptr<MovementComponentData> &movementComponentData);

      ~NoPhysicsMovementComponent() override;

      eComponentType GetComponentType() const override;

      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      void Move(const float deltaTime) override;

      void Move(const glm::vec3 &direction, const float deltaTime) override;

      void Jump() override;

      void Teleport(const glm::vec3 &teleportPosition) override;

      void PostLevelInit() override;

      glm::vec3 GetVelocity() const;
   };

}
