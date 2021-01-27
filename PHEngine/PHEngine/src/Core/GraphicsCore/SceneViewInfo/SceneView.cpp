#include "SceneView.h"

#include <iostream>

namespace Graphics
{
   SceneView::SceneView(std::shared_ptr<CameraSceneProxy> cameraProxy, const std::unordered_map<size_t, std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
      : mCameraProxy(cameraProxy)
      , mPrimitiveProxies(primitiveProxies)
   {
   }

   SceneView::~SceneView()
   {
   }

   void SceneView::DoVisibilityTest()
   {
      for (const auto& primitiveProxyPair : mPrimitiveProxies)
      {
         auto proxy = primitiveProxyPair.second;

         bool bProxyVisible = true;

         if (proxy->IsFrustumCullTestNeeded())
         {
             bProxyVisible = mCameraProxy->GetCameraFrustum().CollidesWithBoundingBox(proxy->GetTransformedBoundingBox());
         }

         mVisibilityMap[primitiveProxyPair.first] = bProxyVisible;
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
