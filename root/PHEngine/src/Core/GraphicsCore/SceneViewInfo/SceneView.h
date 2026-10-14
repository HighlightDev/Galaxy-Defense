#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace Graphics {
class CameraSceneProxy;

namespace Proxy {
class PrimitiveSceneProxy;
}

class SceneView {
    std::shared_ptr<CameraSceneProxy> mCameraProxySp;

    std::unordered_map<int32_t /*proxy id*/, bool> mVisibilityMap;

#if DEBUG
    int32_t mLastFrameVisiblePrimitives{0};
#endif

public:
    explicit SceneView(const std::shared_ptr<CameraSceneProxy>& cameraProxy);

    ~SceneView();

    std::shared_ptr<CameraSceneProxy> GetCameraProxy() const;

    void FrustumCullTest(const std::vector<std::shared_ptr<::Graphics::Proxy::PrimitiveSceneProxy>>& primitiveProxies);

    bool IsPrimitiveVisible(const int32_t proxyId) const;

    void ResetVisibilityForPrimitive(const int32_t proxyId);

    // should be called when all proxies must be retest for visibility (camera transform was changed)
    void ResetVisibility();
};
} // namespace Graphics
