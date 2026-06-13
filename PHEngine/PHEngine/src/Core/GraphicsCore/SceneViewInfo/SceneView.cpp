#include "SceneView.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

using namespace Graphics::Proxy;

namespace Graphics {
SceneView::SceneView(const std::shared_ptr<CameraSceneProxy>& cameraProxy)
    : mCameraProxySp(cameraProxy)
    , mVisibilityMap()
{
}

SceneView::~SceneView()
{
}

void SceneView::FrustumCullTest(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
{
    if (not mCameraProxySp->IsCameraFrustumBuilt()) {
        for (const auto& proxy : primitiveProxies) {
            mVisibilityMap[proxy->GetSceneProxyId()] = true;
        }
        LogInfo("SceneView::FrustumCullTest: CameraFrustum wasn't yet built.");
        return;
    }

    // Only test proxies which are enabled, visible, have a built world matrix and are missing in the visibility map.
    const auto& cameraFrustum = mCameraProxySp->GetCameraFrustum();
    for (const auto& proxy : primitiveProxies) {
        const int32_t proxyId = proxy->GetSceneProxyId();
        if (!proxy->IsEnabled() || !proxy->IsVisible() || !proxy->IsTransformIntialized() || mVisibilityMap.contains(proxyId)) {
            continue;
        }

        mVisibilityMap[proxyId]
            = proxy->IsFrustumCullTestNeeded() ? cameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox()) : true;
    }

#if DEBUG
    int32_t currentFrameVisiblePrimitives = 0;
    for (const auto& [proxyId, isVisible] : mVisibilityMap) {
        currentFrameVisiblePrimitives += static_cast<int32_t>(isVisible);
    }
    if (currentFrameVisiblePrimitives != mLastFrameVisiblePrimitives) {
        EngineCore::LogInfo("SceneView::FrustumCullTest: visible primitives: ", currentFrameVisiblePrimitives);
    }
    mLastFrameVisiblePrimitives = currentFrameVisiblePrimitives;
#endif
}

std::shared_ptr<CameraSceneProxy> SceneView::GetCameraProxy() const
{
    return mCameraProxySp;
}

bool SceneView::IsPrimitiveVisible(const int32_t proxyId) const
{
    if (mVisibilityMap.contains(proxyId)) {
        return mVisibilityMap.at(proxyId);
    }
    return false;
}

void SceneView::ResetVisibilityForPrimitive(const int32_t proxyId)
{
    if (mVisibilityMap.contains(proxyId)) {
        mVisibilityMap.erase(proxyId);
    }
}

void SceneView::ResetVisibility()
{
    if (not mVisibilityMap.empty()) {
        mVisibilityMap.clear();
    }
}

} // namespace Graphics
