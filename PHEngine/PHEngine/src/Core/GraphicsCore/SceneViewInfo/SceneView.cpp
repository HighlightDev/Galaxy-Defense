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
      size_t count = 0;
      for (const auto& primitiveProxyPair : mPrimitiveProxies)
      {
         const size_t primitiveIndex = primitiveProxyPair.first;
         auto proxy = primitiveProxyPair.second;

         const auto& boundingBox = proxy->GetTransformedBoundingBox();
         mVisibilityMap[primitiveIndex] = mCameraProxy->GetCameraFrustum().IsIntersectionWithBoundingBox(boundingBox, true);

         if (!mVisibilityMap.at(primitiveIndex))
            count++;
      }
      std::cout << "Frustum culled: " << count << std::endl;
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
