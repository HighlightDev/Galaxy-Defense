#include "MainThirdPersonCamera.h"

#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"

namespace Game 
{

   MainThirdPersonCamera::MainThirdPersonCamera(const std::string& cameraName, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort,
      const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget, const glm::vec3& thirdPersonTargetOffset)
      : ThirdPersonCamera(cameraName, scene, viewPort, initPitchDeg, initYawDeg, camDistanceToThirdPersonTarget, thirdPersonTargetOffset)
   {
      MouseMovedEvent::GetInstance()->AddListener(this);
      m_cameraType = ACamera::CameraType::MAIN_THIRD_PERSON_CAMERA;
   }

   MainThirdPersonCamera::~MainThirdPersonCamera()
   {
      MouseMovedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<CameraSceneProxy> MainThirdPersonCamera::CreateSceneProxy() const
   {
      return std::make_shared<MainCameraSceneProxy>(this);
   }

   void MainThirdPersonCamera::ProcessEvent(const typename MouseMovedEvent::EventData_t& data)
   {
      const auto& mouseData = std::get<0>(data);
      SetRotation(mouseData.z, mouseData.w); // Rotate the camera
   }

}
