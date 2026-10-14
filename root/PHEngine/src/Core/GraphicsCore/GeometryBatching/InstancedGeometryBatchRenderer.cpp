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
    ext_assert(batchKey != "", "InstancedGeometryBatchRenderer::TryToAddBatchProxy: batchKey is empty");
    if (!mBatchProxies.count(batchKey)) {
        mBatchProxies.emplace(batchKey, batch);
        return true;
    }
    return false;
}

bool InstancedGeometryBatchRenderer::CheckIfBatchProxyExists(const std::string& batchKey) const
{
    return mBatchProxies.find(batchKey) != mBatchProxies.cend();
}

std::shared_ptr<InstancedGeometryBatchProxy> InstancedGeometryBatchRenderer::GetBatchProxy(const std::string& batchKey) const
{
    ext_assert(not batchKey.empty(), "InstancedGeometryBatchRenderer::GetBatchProxy: batchKey is empty");
    const auto foundIt = mBatchProxies.find(batchKey);
    return foundIt != mBatchProxies.cend() ? foundIt->second : nullptr;
}

void InstancedGeometryBatchRenderer::RemoveBatchProxy(const std::string& batchKey)
{
    const auto foundIt = mBatchProxies.find(batchKey);
    ext_assert(foundIt != mBatchProxies.cend(), "Batch proxy with sought batch name doesn't exist.");
    if (foundIt != mBatchProxies.cend()) {
        mBatchProxies.erase(foundIt);
    }
}

void InstancedGeometryBatchRenderer::RenderAllBatches(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState,
    const eInstancedGeometryBatchRenderType renderType)
{
    if (not mBatchProxies.empty()) {
        for (const auto& [_, batchProxySp] : mBatchProxies) {
            if (batchProxySp->IsDeferred() and renderType != eInstancedGeometryBatchRenderType::FORWARD
                || not batchProxySp->IsDeferred() and renderType == eInstancedGeometryBatchRenderType::FORWARD) {
                batchProxySp->Render(cameraSceneProxy, viewMatrix, projectionMatrix, activeBindedState);
            }
        }
    }
}

void InstancedGeometryBatchRenderer::UpdateBatchInstancesData(std::unordered_map<std::string, std::vector<int32_t>>&& data)
{
    for (auto&& [updatedBatchKey, udpatedBatchProxyData] : data) {
        const auto foundIt = mBatchProxies.find(updatedBatchKey);
        ext_assert(
            foundIt != mBatchProxies.cend(),
            "InstancedGeometryBatchRenderer::UpdateBatchInstancesData: No batch found for batchKey: " + updatedBatchKey);
        if (foundIt != mBatchProxies.cend()) {
            foundIt->second->UpdateValidInstances(std::move(udpatedBatchProxyData));
        }
    }
}

void InstancedGeometryBatchRenderer::CleanUp()
{
    mBatchProxies.clear();
}
} // namespace Graphics::GeometryBatching
