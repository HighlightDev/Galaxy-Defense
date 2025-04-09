#include "InstancedGeometryBatchRenderer.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Renderer/ActiveBindedState.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "InstancedGeometryBatchProxy.h"

using namespace Graphics;

namespace Graphics::GeometryBatching {
bool InstancedGeometryBatchRenderer::TryToAddBatchProxy(const std::shared_ptr<InstancedGeometryBatchProxy>& batch)
{
    const auto& batchKey = batch->GetBatchKey();
    assert(batchKey != "");
    if (!mBatchProxies.count(batchKey)) {
        mBatchProxies.emplace(batchKey, batch);
        return true;
    }
    return false;
}

bool InstancedGeometryBatchRenderer::CheckIfBatchProxyExists(const std::string& batchKey) const
{
    return mBatchProxies.count(batchKey) > 0;
}

std::shared_ptr<InstancedGeometryBatchProxy> InstancedGeometryBatchRenderer::GetBatchProxy(const std::string& batchKey) const
{
    assert(batchKey != "");
    if (mBatchProxies.count(batchKey)) {
        return mBatchProxies.at(batchKey);
    }
    return nullptr;
}

void InstancedGeometryBatchRenderer::RemoveBatchProxy(const std::string& batchKey)
{
    ext_assert(mBatchProxies.count(batchKey), "Batch proxy with sought batch name doesn't exist.");
    mBatchProxies.erase(batchKey);
}

void InstancedGeometryBatchRenderer::RenderAllBatches(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    for (const auto& [key, batchProxySp] : mBatchProxies) {
        batchProxySp->Render(cameraSceneProxy, viewMatrix, projectionMatrix, activeBindedState);
    }
}

void InstancedGeometryBatchRenderer::UpdateBatchInstancesData(const std::unordered_map<std::string, std::vector<int32_t>>& data)
{
    for (const auto& [batchKey, batchProxySp] : mBatchProxies) {
        assert(data.count(batchKey));
        batchProxySp->UpdateValidInstances(data.at(batchKey));
    }
}

void InstancedGeometryBatchRenderer::CleanUp()
{
    mBatchProxies.clear();
}
} // namespace Graphics::GeometryBatching
