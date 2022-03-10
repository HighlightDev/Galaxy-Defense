#pragma once
#include "MovementComponent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

using namespace Event;
using namespace EnginePhysics;

namespace EngineCore
{
   class MovementComponentData;

   class HumanoidPhysicsMovementComponent
       : public MovementComponent,
         public CameraTransformChangedEvent
   {

      float mCameraYaw;
      float mCameraPitch;

      std::string mCameraName;

      bool bIsCameraRotationDirty = false;

     std::shared_ptr<CharacterPhysicsComponent> m_playerPhysicsComponent;

   public:
      HumanoidPhysicsMovementComponent(const MovementComponentData& movementComponentData);

      virtual ~HumanoidPhysicsMovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      virtual void ProcessEvent(const typename CameraTransformChangedEvent::EventData_t &data) override;

      virtual void Move() override;

      virtual void Jump() override;

      virtual void Teleport(const glm::vec3& teleportPosition) override;

      virtual void PostLevelInit() override;

   protected:
      glm::vec3 GetVelocity() const;

      glm::mat3 GetCameraYawRotationMatrix() const;

      glm::vec3 GetCameraPitchYawRoll() const;

   };

}
