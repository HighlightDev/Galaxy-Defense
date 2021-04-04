#pragma once
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <memory>
#include <vector>
#include <unordered_map>

using namespace Graphics::Proxy;

namespace Graphics
{
   class SceneView
   {
      std::shared_ptr<CameraSceneProxy> mCameraProxy;

      const std::unordered_map<size_t, std::shared_ptr<PrimitiveSceneProxy>>& mPrimitiveProxies;

      std::unordered_map<size_t/*proxy id*/, bool> mVisibilityMap;

   public:

      SceneView(std::shared_ptr<CameraSceneProxy> cameraProxy, const std::unordered_map<size_t, std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies);

      ~SceneView();

      std::shared_ptr<CameraSceneProxy> GetCameraProxy() const;

      void DoVisibilityTest();

      bool IsPrimitiveVisible(const size_t proxyId) const;

      std::unordered_map<size_t/*proxy id*/, bool> GetVisibilityForTransformedFrustum(const glm::mat4& transformMatrix);

   };
}
