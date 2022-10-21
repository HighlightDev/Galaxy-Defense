#include "SceneView.h"

#include <iostream>

namespace Graphics
{
   SceneView::SceneView(const std::shared_ptr<CameraSceneProxy> &cameraProxy, const std::vector<std::shared_ptr<PrimitiveSceneProxy>> &primitiveProxies)
       : mCameraProxy(cameraProxy),
         mPrimitiveProxies(primitiveProxies),
         mVisibilityMap()
   {
   }

   SceneView::~SceneView()
   {
   }

   void SceneView::DoVisibilityTest()
   {
      if (!mCameraProxy->IsCameraFrustumBuilt())
         return;

      const auto frustum = mCameraProxy->GetCameraFrustum();

      for (const auto &proxy : mPrimitiveProxies)
      {
         mVisibilityMap[proxy->GetSceneProxyId()] = proxy->IsFrustumCullTestNeeded() ?
            frustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox()) : true;
      }
   }

   std::shared_ptr<CameraSceneProxy> SceneView::GetCameraProxy() const
   {
      return mCameraProxy;
   }

   bool SceneView::IsPrimitiveVisible(const size_t proxyId) const
   {
      assert(mVisibilityMap.count(proxyId));
      return mVisibilityMap.at(proxyId);
   }
}
