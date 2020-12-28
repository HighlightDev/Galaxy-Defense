#pragma once
#include "CameraSceneProxy.h"

namespace Game
{
   class ACamera;
}

namespace Graphics
{

   using Game::ACamera;

   class MainCameraSceneProxy :
      public CameraSceneProxy
   {
   public:

      MainCameraSceneProxy(const class ACamera* camera);

      virtual ~MainCameraSceneProxy();

      virtual eCameraSceneProxyType GetCameraSceneType() const override;
   };
}
