#pragma once
#include "Component.h"

#include <glm/vec3.hpp>

namespace Game
{

   class MovementComponent :
      public Component
   {

      class ICamera* const m_mainCamera;

   public:

      glm::vec3 Velocity;

      float Speed;

      MovementComponent(class  ICamera* const mainCamera, glm::vec3 launchVelocity);

      virtual ~MovementComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      class  ICamera* const GetCamera() const;

      glm::vec3 GetMoveOffset() const;
   };

}
