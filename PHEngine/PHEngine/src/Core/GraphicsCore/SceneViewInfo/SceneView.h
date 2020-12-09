#pragma once
#include "Core/GraphicsCore/SceneViewInfo/AVisiblePrimitiveBase.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"

#include <memory>
#include <vector>

namespace Graphics
{
   class SceneView
   {
      std::shared_ptr<CameraSceneProxy> mCameraProxy;

   public:

      SceneView(std::shared_ptr<CameraSceneProxy> cameraProxy);

      ~SceneView();

      void DoVisibilityTest(std::shared_ptr<CameraSceneProxy> cameraProxy, std::vector<std::shared_ptr<AVisiblePrimitiveBase>> testedPrimitives);

      std::shared_ptr<CameraSceneProxy> GetCameraProxy() const;
   };
}
