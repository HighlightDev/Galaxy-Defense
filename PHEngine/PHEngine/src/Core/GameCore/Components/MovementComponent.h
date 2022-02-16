#pragma once
#include "Component.h"

#include <glm/vec3.hpp>

namespace Game
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

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) = 0;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) = 0;

      virtual void Move() = 0;

      virtual void Jump() = 0;

      float GetSpeed() const;

      void SetSpeed(const float speed);

      void SetDirection(const glm::vec3& direction);

      glm::vec3 GetDirection() const;
   };

}
