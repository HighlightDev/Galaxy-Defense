#include "CharacterPhysicsMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"

namespace Game
{

   CharacterPhysicsMovementComponent::CharacterPhysicsMovementComponent(const std::string &gameObjectName, const glm::vec3 &launchDirection, const std::string &cameraName)
       : MovementComponent(gameObjectName), CameraTransformChangedEvent(), mCameraName(cameraName), mDirection(launchDirection), m_playerPhysicsComponent()
   {
      CameraTransformChangedEvent::GetInstance()->AddListener(this);
   }

   void CharacterPhysicsMovementComponent::PostLevelInit()
   {
      if (const auto &spOwner = GetOwner().lock())
      {
         m_playerPhysicsComponent = std::static_pointer_cast<CharacterPhysicsComponent>(spOwner->GetPhysicsComponent());
         assert(m_playerPhysicsComponent);
      }
   }

   CharacterPhysicsMovementComponent::~CharacterPhysicsMovementComponent()
   {
      CameraTransformChangedEvent::GetInstance()->RemoveListener(this);
   }

   ComponentType CharacterPhysicsMovementComponent::GetComponentType() const
   {
      return CHARACTER_MOVEMENT_COMPONENT;
   }

   void CharacterPhysicsMovementComponent::Tick(const float deltaTime)
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

   void CharacterPhysicsMovementComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataCharacterMovementComponent> data = std::make_shared<SerializeDataCharacterMovementComponent>();
      data->ComponentName = GameObjectName;
      data->CameraName = mCameraName;
      data->LaunchDirection = mDirection;
      actorData.ComponentsData.emplace_back(data);
   }

   void CharacterPhysicsMovementComponent::ProcessEvent(const CameraTransformChangedEvent::EventData_t &data)
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

   void CharacterPhysicsMovementComponent::Move()
   {
      m_playerPhysicsComponent->SetWalkVelocity(GetVelocity());
   }

   void CharacterPhysicsMovementComponent::Jump()
   {
      m_playerPhysicsComponent->SetJumpVelocity();
   }

   glm::vec3 CharacterPhysicsMovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }

   glm::mat3 CharacterPhysicsMovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 CharacterPhysicsMovementComponent::GetCameraPitchYawRoll() const
   {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
