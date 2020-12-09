#include "SceneView.h"

namespace Graphics
{
   SceneView::SceneView(std::shared_ptr<CameraSceneProxy> cameraProxy)
      : mCameraProxy(cameraProxy)
   {
   }

   SceneView::~SceneView()
   {
   }

   void SceneView::DoVisibilityTest(std::shared_ptr<CameraSceneProxy> cameraProxy, std::vector<std::shared_ptr<AVisiblePrimitiveBase>> testedPrimitives)
   {
      // todo: do visibility testing 
   }

   std::shared_ptr<CameraSceneProxy> SceneView::GetCameraProxy() const
   {
      return mCameraProxy;
   }
}
