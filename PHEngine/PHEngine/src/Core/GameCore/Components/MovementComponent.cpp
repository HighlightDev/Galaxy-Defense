#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   MovementComponent::MovementComponent(const glm::vec3& launchDirection, const std::string& cameraName)
      : Component()
      , CameraTransformChangedEvent()
      , mCameraName(cameraName)
      , mDirection(launchDirection)
      , mSpeed(4.0f)
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

         mDirection = cameraPtr->GetEyeSpaceForwardVector();

         mCameraYaw = cameraPtr->GetRotationYaw();
         mCameraPitch = cameraPtr->GetRotationPitch();
      }
   }

   float MovementComponent::GetSpeed() const
   {
      return mSpeed;
   }

   void MovementComponent::SetSpeed(const float speed)
   {
      mSpeed = speed;
   }

   glm::vec3 MovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }

   glm::mat3 MovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 MovementComponent::GetCameraPitchYawRoll() const {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
