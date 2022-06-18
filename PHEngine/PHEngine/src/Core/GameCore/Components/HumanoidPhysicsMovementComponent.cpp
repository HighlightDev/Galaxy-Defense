#include "HumanoidPhysicsMovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

namespace EngineCore
{

   HumanoidPhysicsMovementComponent::HumanoidPhysicsMovementComponent(const MovementComponentData& movementComponentData)
       : MovementComponent(movementComponentData), CameraTransformChangedEvent(), mCameraName(""), m_playerPhysicsComponent()
   {
      CameraTransformChangedEvent::GetInstance()->AddListener(this);
      const auto& charMoveCompData = static_cast<const HumanoidMovementComponentData&>(movementComponentData);
      mCameraName = charMoveCompData.mCameraName;
   }

   void HumanoidPhysicsMovementComponent::PostLevelInit()
   {
      MovementComponent::PostLevelInit();
      
      if (const auto &spOwner = GetOwner().lock())
      {
         m_playerPhysicsComponent = std::static_pointer_cast<CharacterPhysicsComponent>(spOwner->GetPhysicsComponent());
         assert(m_playerPhysicsComponent);
      }
   }

   HumanoidPhysicsMovementComponent::~HumanoidPhysicsMovementComponent()
   {
      CameraTransformChangedEvent::GetInstance()->RemoveListener(this);
   }

   eComponentType HumanoidPhysicsMovementComponent::GetComponentType() const
   {
      return CHARACTER_MOVEMENT_COMPONENT;
   }

   void HumanoidPhysicsMovementComponent::Tick(const float deltaTime)
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

   void HumanoidPhysicsMovementComponent::Teleport(const glm::vec3& teleportPosition)
   {
   }

   void HumanoidPhysicsMovementComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataCharacterMovementComponent> data = std::make_shared<SerializeDataCharacterMovementComponent>();
      data->ComponentName = GameObjectName;
      data->CameraName = mCameraName;
      data->LaunchDirection = mDirection;
      actorData.ComponentsData.emplace_back(data);
   }

   void HumanoidPhysicsMovementComponent::ProcessEvent(const CameraTransformChangedEvent::EventData_t &data)
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

   void HumanoidPhysicsMovementComponent::Move()
   {
      m_playerPhysicsComponent->SetWalkVelocity(GetVelocity());
   }

   void HumanoidPhysicsMovementComponent::Jump()
   {
      m_playerPhysicsComponent->SetJumpVelocity();
   }

   glm::vec3 HumanoidPhysicsMovementComponent::GetVelocity() const
   {
      return mDirection * mSpeed;
   }

   glm::mat3 HumanoidPhysicsMovementComponent::GetCameraYawRotationMatrix() const
   {
      return glm::rotate(glm::mat4(1), DEG_TO_RAD(mCameraYaw), AXIS_UP);
   }

   glm::vec3 HumanoidPhysicsMovementComponent::GetCameraPitchYawRoll() const
   {
      return glm::vec3(mCameraPitch, mCameraYaw, 0.0f);
   }
}
