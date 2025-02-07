#include "SceneView.h"

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
    if (mCameraProxy->IsCameraFrustumBuilt()) {
        const auto& cameraFrustum = mCameraProxy->GetCameraFrustum();
        for (const auto& proxy : mPrimitiveProxies) {
            mVisibilityMap[proxy->GetSceneProxyId()] = proxy->IsFrustumCullTestNeeded()
                ? cameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox())
                : true;
        }
    } else {
        for (const auto& proxy : mPrimitiveProxies) {
            mVisibilityMap[proxy->GetSceneProxyId()] = true;
        }
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
} // namespace Graphics
