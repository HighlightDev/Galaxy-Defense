#pragma once

#include "Core/GraphicsCore/VisibilityManager/CameraFrustum.h"

namespace Game
{
   class ACamera;
}

namespace Graphics
{
   using Game::ACamera;

   class CameraSceneProxy
   {

      CameraFrustum mCameraFrustum;

   public:

      CameraSceneProxy(const class ACamera* camera);
   };

}
