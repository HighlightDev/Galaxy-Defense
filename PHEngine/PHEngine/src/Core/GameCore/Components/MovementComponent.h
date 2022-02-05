#pragma once
#include "Component.h"

namespace Game
{
   class Actor;

   class MovementComponent
       : public Component
   {
   protected:
   
      float mSpeed;

   public:

      MovementComponent(const std::string &gameObjectName);

      virtual ~MovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) = 0;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) = 0;

      virtual void Move() = 0;

      virtual void Jump() = 0;

      float GetSpeed() const;

      void SetSpeed(const float speed);
   };

}
