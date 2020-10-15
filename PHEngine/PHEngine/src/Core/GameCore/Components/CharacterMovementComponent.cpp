#include "CharacterMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   CharacterMovementComponent::CharacterMovementComponent(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
      : Component(gameObjectName)
      , CameraTransformChangedEvent()
      , mCameraName(cameraName)
      , mDirection(launchDirection)
      , mSpeed(4.0f)
   {
      CameraTransformChangedEvent::GetInstance()->AddListener(this);
   }

   CharacterMovementComponent::~CharacterMovementComponent()
   {
      CameraTransformChangedEvent::GetInstance()->RemoveListener(this);
   }

   ComponentType CharacterMovementComponent::GetComponentType() const
   {
      return CHARACTER_MOVEMENT_COMPONENT;
   }

   void CharacterMovementComponent::Tick(const float deltaTime)
   {
      
   }

   void CharacterMovementComponent::ProcessEvent(const CameraTransformChangedEvent::EventData_t& data)
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

   float CharacterMovementComponent::GetSpeed() const
   {
      return mSpeed;
   }

   void CharacterMovementComponent::SetSpeed(const float speed)
   {
      mSpeed = speed;
   }

   glm::vec3 CharacterMovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }

   glm::mat3 CharacterMovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 CharacterMovementComponent::GetCameraPitchYawRoll() const {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
