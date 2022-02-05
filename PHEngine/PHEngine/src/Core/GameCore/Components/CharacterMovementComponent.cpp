#include "CharacterMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"

namespace Game
{

   CharacterMovementComponent::CharacterMovementComponent(const std::string &gameObjectName, const glm::vec3 &launchDirection, const std::string &cameraName)
       : MovementComponent(gameObjectName), CameraTransformChangedEvent(), mCameraName(cameraName), mDirection(launchDirection), m_playerPhysicsComponent()
   {
      CameraTransformChangedEvent::GetInstance()->AddListener(this);
   }

   void CharacterMovementComponent::PostLevelInit()
   {
      if (const auto &spOwner = GetOwner().lock())
      {
         m_playerPhysicsComponent = std::static_pointer_cast<CharacterPhysicsComponent>(spOwner->GetPhysicsComponent());
         assert(m_playerPhysicsComponent);
      }
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
      if (bIsCameraRotationDirty)
      {
         if (const auto &spOwner = GetOwner().lock())
         {
            spOwner->GetRootComponent()->SetAdditionalRotation(GetCameraPitchYawRoll());
         }

         bIsCameraRotationDirty = false;
      }
   }

   void CharacterMovementComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataCharacterMovementComponent> data = std::make_shared<SerializeDataCharacterMovementComponent>();
      data->ComponentName = GameObjectName;
      data->CameraName = mCameraName;
      data->LaunchDirection = mDirection;
      actorData.ComponentsData.emplace_back(data);
   }

   void CharacterMovementComponent::ProcessEvent(const CameraTransformChangedEvent::EventData_t &data)
   {
      auto &cameraPtr = std::get<0>(data);

      if (cameraPtr->GetCameraName() == mCameraName)
      {
         mDirection = cameraPtr->GetEyeSpaceForwardVector();
         mCameraYaw = cameraPtr->GetRotationYaw();
         mCameraPitch = cameraPtr->GetRotationPitch();

         bIsCameraRotationDirty = true;
      }
   }

   void CharacterMovementComponent::Move()
   {
      m_playerPhysicsComponent->SetWalkVelocity(GetVelocity());
   }

   void CharacterMovementComponent::Jump()
   {
      m_playerPhysicsComponent->SetJumpVelocity();
   }

   glm::vec3 CharacterMovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }

   glm::mat3 CharacterMovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 CharacterMovementComponent::GetCameraPitchYawRoll() const
   {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
