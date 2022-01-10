#pragma once
#include "Component.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

using namespace Event;

namespace Game
{

   class CharacterMovementComponent
      : public Component
      , public CameraTransformChangedEvent
   {

      float mCameraYaw;
      float mCameraPitch;

      std::string mCameraName;

      bool bIsCameraRotationDirty = false;

      glm::vec3 mDirection;

      float mSpeed;

   public:

      CharacterMovementComponent(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName);

      virtual ~CharacterMovementComponent();

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual void ProcessEvent(const typename CameraTransformChangedEvent::EventData_t& data) override;

      glm::vec3 GetVelocity() const;

      glm::mat3 GetCameraYawRotationMatrix() const;

      glm::vec3 GetCameraPitchYawRoll() const;

      float GetSpeed() const;

      void SetSpeed(const float speed);

      inline bool GetIsCameraRotationDirty() const {
         return bIsCameraRotationDirty;
      }

      inline void SetIsCameraRotationDirty(const bool bCamRotDirty) {
         bIsCameraRotationDirty = bCamRotDirty;
      }
   };

}
