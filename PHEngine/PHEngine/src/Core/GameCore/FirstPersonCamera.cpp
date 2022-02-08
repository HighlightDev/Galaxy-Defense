#include "FirstPersonCamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"

#include <set>

using namespace Graphics;

namespace Game
{

   FirstPersonCamera::FirstPersonCamera(const std::string &cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg, const float initYawDeg, glm::vec3 camPos)
       : ACamera(cameraName, cameraType, scene, viewPort, initPitchDeg, initYawDeg)
       , m_firstPersonCameraPosition(camPos)
       , m_cameraMoveSpeed(0.01f)
       , mInputComponent(std::make_unique<InputComponent>("FirstPersonCameraInputComponent"))
   {
      ACamera::UpdateRotationMatrix(0, 0);
   }

   FirstPersonCamera::~FirstPersonCamera()
   {
   }

   glm::vec3 FirstPersonCamera::GetEyeVector() const
   {
      return m_firstPersonCameraPosition;
   }

   glm::vec3 FirstPersonCamera::GetTargetVector() const
   {
      return m_firstPersonCameraPosition + (m_eyeSpaceForwardVector * m_cameraMoveSpeed);
   }

   glm::vec3 FirstPersonCamera::GetLocalSpaceUpVector() const
   {
      return m_localSpaceUpVector;
   }

   void FirstPersonCamera::Tick(const float DeltaTime)
   {
      static const std::map<Keys, int32_t> mappingDirections = {{Keys::W, 0}, {Keys::D, 4}, {Keys::A, 3}, {Keys::S, 1}};

      auto &mouseBindings = mInputComponent->GetMouseBindings();
      if (mouseBindings.IsMouseMoveEventDirty())
      {
         const auto &mouseMoveEvent = mouseBindings.FlushMouseMoveEvent();
         SetRotation(mouseMoveEvent.z, mouseMoveEvent.w);
      }

      const auto &keyboardBindings = mInputComponent->GetKeyboardBindings();
      if (keyboardBindings.HasPressedKeys())
      {
         int32_t moveDirection = -1;
         if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
         {
            moveDirection = 0;
         }
         else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
         {
            moveDirection = 2;
         }
         else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
         {
            moveDirection = 3;
         }
         else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
         {
            moveDirection = 1;
         }

         MoveCamera(moveDirection);

         const std::vector<eKeyActionType> &currentFrameReleasedKeys = mInputComponent->GetReleasedKeyActions();
         const std::vector<eKeyActionType> &currentFramePressedKeys = mInputComponent->GetPressedKeyActions();
      }

      ACamera::Tick(DeltaTime);
   }

   std::shared_ptr<CameraSceneProxy> FirstPersonCamera::CreateSceneProxy() const
   {
      if (eCameraType::MAIN_FIRST_PERSON_CAMERA == m_cameraType)
      {
         return std::make_shared<MainCameraSceneProxy>(this);
      }
      else
      {
         return std::make_shared<CameraSceneProxy>(this);
      }
   }

   std::string FirstPersonCamera::GetCameraTypeName() const
   {
      return eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType ? "MainFirstPersonCamera" : "FirstPersonCamera";
   }

   void FirstPersonCamera::MoveCamera(int32_t direction)
   {
      switch (direction)
      {
      case 0:
         m_firstPersonCameraPosition += GetEyeSpaceForwardVector() * m_cameraMoveSpeed;
         break;
      case 1:
         m_firstPersonCameraPosition -= GetEyeSpaceForwardVector() * m_cameraMoveSpeed;
         break;
      case 2:
         m_firstPersonCameraPosition -= GetEyeSpaceRightVector() * m_cameraMoveSpeed;
         break;
      case 3:
         m_firstPersonCameraPosition += GetEyeSpaceRightVector() * m_cameraMoveSpeed;
         break;
      }
   }

   void FirstPersonCamera::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      const auto &cameraData = SerializeHelper::GetSerializedDataCamera(this);
      dataContainer.Cameras.emplace_back(cameraData);
   }

   float FirstPersonCamera::GetCameraMoveSpeed() const
   {
      return m_cameraMoveSpeed;
   }
}
