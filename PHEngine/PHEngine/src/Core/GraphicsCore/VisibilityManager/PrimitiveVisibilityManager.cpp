#include "PrimitiveVisibilityManager.h"

namespace Graphics
{
   PrimitiveVisibilityManager::PrimitiveVisibilityManager(std::shared_ptr<CameraFrustum> cameraFrustum)
      : mCameraFrustum(cameraFrustum)
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
