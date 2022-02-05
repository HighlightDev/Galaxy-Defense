#pragma once
#include "MovementComponent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

using namespace Event;
using namespace EnginePhysics;

namespace Game
{

   class CharacterMovementComponent
       : public MovementComponent,
         public CameraTransformChangedEvent
   {

      float mCameraYaw;
      float mCameraPitch;

      std::string mCameraName;

      bool bIsCameraRotationDirty = false;

      glm::vec3 mDirection;

     std::shared_ptr<CharacterPhysicsComponent> m_playerPhysicsComponent;

   public:
      CharacterMovementComponent(const std::string &gameObjectName, const glm::vec3 &launchDirection, const std::string &cameraName);

      virtual ~CharacterMovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      virtual void ProcessEvent(const typename CameraTransformChangedEvent::EventData_t &data) override;

      virtual void Move() override;

      virtual void Jump() override;

      virtual void PostLevelInit() override;

   protected:
      glm::vec3 GetVelocity() const;

      glm::mat3 GetCameraYawRotationMatrix() const;

      glm::vec3 GetCameraPitchYawRoll() const;

   };

}
