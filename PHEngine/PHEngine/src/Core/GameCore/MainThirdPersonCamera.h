#pragma once
#include "ThirdPersonCamera.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"

using namespace Event;

namespace Game
{
   class Scene;

   class MainThirdPersonCamera
      : public ThirdPersonCamera
      , public MouseMovedEvent
   {
   public:

      MainThirdPersonCamera(const std::string& cameraName, std::shared_ptr<Scene> scene, const ViewPortInfo& viewPort,
         const float initPitchDeg, const float initYawDeg, const float camDistanceToThirdPersonTarget);

      ~MainThirdPersonCamera();

      virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;

      virtual void ProcessEvent(const typename MouseMovedEvent::EventData_t& data) override;
   };
}

