#include "PrimitiveVisibilityManager.h"

namespace Graphics
{
   PrimitiveVisibilityManager::PrimitiveVisibilityManager(std::shared_ptr<CameraSceneProxy> cameraProxy)
      : mCameraProxy(cameraProxy)
   {
   }

   PrimitiveVisibilityManager::~PrimitiveVisibilityManager()
   {
   }

   void PrimitiveVisibilityManager::DoVisibilityTest(std::shared_ptr<CameraSceneProxy> cameraProxy, std::vector<std::shared_ptr<AVisibleBase>> testedPrimitives)
   {
      // todo: do visibility testing 
   }
}
