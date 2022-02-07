#include "FirstPersonCamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"
#include "Core/GameCore/Input/Keys.h"

using namespace Graphics;

namespace Game
{

   FirstPersonCamera::FirstPersonCamera(const std::string &cameraName, const eCameraType cameraType, std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg, const float initYawDeg, glm::vec3 camPos)
       : ACamera(cameraName, cameraType, scene, viewPort, initPitchDeg, initYawDeg), m_firstPersonCameraPosition(camPos), m_cameraMoveSpeed(0.1f)
   {
      KeyboardButtonDownEvent::GetInstance()->AddListener(this);
      MouseMovedEvent::GetInstance()->AddListener(this);
      ACamera::UpdateRotationMatrix(0, 0);
   }

   FirstPersonCamera::~FirstPersonCamera()
   {
      KeyboardButtonDownEvent::GetInstance()->RemoveListener(this);
      MouseMovedEvent::GetInstance()->RemoveListener(this);
   }

   glm::vec3 FirstPersonCamera::GetEyeVector() const
   {
      return m_firstPersonCameraPosition;
   }

   glm::vec3 FirstPersonCamera::GetTargetVector() const
   {
      auto targetVec = m_firstPersonCameraPosition + m_eyeSpaceForwardVector * m_cameraMoveSpeed;
      return targetVec;
   }

   glm::vec3 FirstPersonCamera::GetLocalSpaceUpVector() const
   {
      return m_localSpaceUpVector;
   }

   void FirstPersonCamera::Tick(const float DeltaTime)
   {
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
      case 3:
         m_firstPersonCameraPosition -= GetEyeSpaceRightVector() * m_cameraMoveSpeed;
         break;
      case 4:
         m_firstPersonCameraPosition += GetEyeSpaceRightVector() * m_cameraMoveSpeed;
         break;
      }
   }

   void FirstPersonCamera::ProcessEvent(const KeyboardButtonDownEvent::EventData_t &eventData)
   {
      const auto &data = std::get<0>(eventData);

      static const std::map<Keys, int32_t> mappingDirections = {{Keys::W, 0}, {Keys::D, 4}, {Keys::A, 3}, {Keys::S, 1}};

      if (data.State == KeyState::PRESSED)
      {
         if (mappingDirections.count(data.Key))
         {
            MoveCamera(mappingDirections.at(data.Key));
         }
      }
   }

   void FirstPersonCamera::ProcessEvent(const MouseMovedEvent::EventData_t &eventData)
   {
      const glm::ivec4 &mouseMoveData = std::get<0>(eventData);
      SetRotation(mouseMoveData.z, mouseMoveData.w);
   }

   void FirstPersonCamera::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto cameraData = SerializeHelper::GetSerializedDataCamera(this);
      dataContainer.Cameras.emplace_back(cameraData);
   }
}
