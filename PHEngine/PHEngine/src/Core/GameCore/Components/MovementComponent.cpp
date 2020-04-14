#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   MovementComponent::MovementComponent(const std::string& cameraName, glm::vec3 launchVelocity)
      : Component()
      , CameraTransformChangedEvent()
      , mCameraName(cameraName)
      , Velocity(launchVelocity)
      , Speed(10)
   {
      CameraTransformChangedEvent::GetInstance()->AddListener(this);
   }

   MovementComponent::~MovementComponent()
   {
      CameraTransformChangedEvent::GetInstance()->RemoveListener(this);
   }

   uint64_t MovementComponent::GetComponentType() const
   {
      return MOVEMENT_COMPONENT;
   }

   void MovementComponent::Tick(const float deltaTime)
   {
      
   }

   void MovementComponent::ProcessEvent(const CameraTransformChangedEvent::EventData_t& data)
   {
      auto& cameraPtr = std::get<0>(data);

      if (cameraPtr->GetCameraName() == mCameraName)
      {
         SetIsCameraRotationDirty(true);

         Velocity = cameraPtr->GetEyeSpaceForwardVector();

         mCameraYaw = cameraPtr->GetRotationYaw();
         mCameraPitch = cameraPtr->GetRotationPitch();
      }
   }

   glm::vec3 MovementComponent::GetMoveOffset() const
   {
      return Velocity * Speed;
   }

   glm::mat3 MovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 MovementComponent::GetCameraPitchYawRoll() const {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
