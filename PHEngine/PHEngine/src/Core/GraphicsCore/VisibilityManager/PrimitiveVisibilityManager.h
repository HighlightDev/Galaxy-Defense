#pragma once
#include "Core/GraphicsCore/VisibilityManager/AVisibleBase.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"

#include <memory>
#include <vector>

namespace Graphics
{
   class PrimitiveVisibilityManager
   {
      std::weak_ptr<CameraSceneProxy> mCameraProxy,

   public:

      PrimitiveVisibilityManager(std::shared_ptr<CameraSceneProxy> cameraProxy);

      ~PrimitiveVisibilityManager();

      void DoVisibilityTest(std::vector<std::shared_ptr<AVisibleBase>> testedPrimitives);
   };
}
