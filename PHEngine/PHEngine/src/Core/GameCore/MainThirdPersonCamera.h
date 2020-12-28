#pragma once
#include "ThirdPersonCamera.h"

namespace Game
{
   class Scene;

   class MainThirdPersonCamera
      : public ThirdPersonCamera
   {
   public:

      MainThirdPersonCamera(const std::string& cameraName, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort,
         const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget);

      ~MainThirdPersonCamera();

      virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;
   };
}

