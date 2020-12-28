#include "MainThirdPersonCamera.h"

#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"

namespace Game 
{

   MainThirdPersonCamera::MainThirdPersonCamera(const std::string& cameraName, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort,
      const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget)
      : ThirdPersonCamera(cameraName, scene, viewPort, initPitchDeg, initYawDeg, camDistanceToThirdPersonTarget)
   {
   }

   MainThirdPersonCamera::~MainThirdPersonCamera()
   {
   }

   std::shared_ptr<CameraSceneProxy> MainThirdPersonCamera::CreateSceneProxy() const
   {
      return std::make_shared<MainCameraSceneProxy>(this);
   }

}
