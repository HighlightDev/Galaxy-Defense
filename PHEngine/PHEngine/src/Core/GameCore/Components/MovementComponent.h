#pragma once
#include "Component.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

using namespace Event;

namespace Game
{

   class MovementComponent
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

      MovementComponent(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName);

      virtual ~MovementComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void ProcessEvent(const CameraTransformChangedEvent::EventData_t& data) override;

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
