#pragma once
#include "CameraSceneProxy.h"

namespace EngineCore
{
   class ACamera;
}

namespace Graphics
{

   using EngineCore::ACamera;

   class MainCameraSceneProxy :
      public CameraSceneProxy
   {
   public:

      MainCameraSceneProxy(const class ACamera* camera);

      virtual ~MainCameraSceneProxy();

      virtual eCameraSceneProxyType GetCameraSceneType() const override;
   };
}
