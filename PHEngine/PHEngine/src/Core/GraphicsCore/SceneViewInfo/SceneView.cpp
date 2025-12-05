#include "SceneView.h"

#include "Core/GameCore/LoggerExtension.h"

namespace Graphics {
SceneView::SceneView(
    const std::shared_ptr<CameraSceneProxy>& cameraProxy,
    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
    : mCameraProxy(cameraProxy)
    , mPrimitiveProxies(primitiveProxies)
    , mVisibilityMap()
{
}

SceneView::~SceneView()
{
}

void SceneView::DoVisibilityTest()
{
    int currentFrameVisiblePrimitives = 0;
    if (mCameraProxy->IsCameraFrustumBuilt()) {
        const auto& cameraFrustum = mCameraProxy->GetCameraFrustum();
        for (const auto& proxy : mPrimitiveProxies) {
            bool proxyVisible = true;
            if (proxy->IsFrustumCullTestNeeded()) {
                proxyVisible = cameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox());
            }
            mVisibilityMap[proxy->GetSceneProxyId()] = proxyVisible;
            currentFrameVisiblePrimitives += (int)proxyVisible;
        }
    } else {
        for (const auto& proxy : mPrimitiveProxies) {
            mVisibilityMap[proxy->GetSceneProxyId()] = true;
        }
        LogInfo("SceneView::DoVisibilityTest: CameraFrustum isn't built.");
    }

    if (currentFrameVisiblePrimitives != mLastFrameVisiblePrimitives) {
        EngineCore::LogInfo("SceneView::DoVisibilityTest: visible primitives: ", currentFrameVisiblePrimitives);
    }
    mLastFrameVisiblePrimitives = currentFrameVisiblePrimitives;
}

std::shared_ptr<CameraSceneProxy> SceneView::GetCameraProxy() const
{
    return mCameraProxy;
}

bool SceneView::IsPrimitiveVisible(const size_t proxyId) const
{
    ext_assert(mVisibilityMap.count(proxyId), "SceneView::IsPrimitiveVisible: Proxy ID not found in visibility map");
    return mVisibilityMap.at(proxyId);
}
} // namespace Graphics
