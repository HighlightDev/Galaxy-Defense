#pragma once
#include "Component.h"

#include <glm/vec3.hpp>

namespace EngineCore
{
   class Actor;
   
   struct MovementComponentData;

   class MovementComponent
       : public Component
   {
   protected:
   
      float mSpeed;

      glm::vec3 mDirection;

   public:

      MovementComponent(const MovementComponentData& movementComponentData);

      virtual ~MovementComponent();

      virtual eComponentType GetComponentType() const override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) = 0;

      virtual void Move() = 0;

      virtual void Jump() = 0;

      virtual void Teleport(const glm::vec3& teleportPosition) = 0;

      float GetSpeed() const;

      void SetSpeed(const float speed);

      void SetDirection(const glm::vec3& direction);

      glm::vec3 GetDirection() const;
   };

}
