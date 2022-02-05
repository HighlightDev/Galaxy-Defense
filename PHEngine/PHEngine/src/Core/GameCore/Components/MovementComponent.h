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

      std::weak_ptr<Actor> mOwner;

   public:

      MovementComponent(const std::string &gameObjectName, std::weak_ptr<Actor> owner);

      virtual ~MovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) = 0;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) = 0;

      virtual void Move() = 0;

      virtual void Jump() = 0;

      virtual void PostLevelInit() override;

      float GetSpeed() const;

      void SetSpeed(const float speed);
   };

}
