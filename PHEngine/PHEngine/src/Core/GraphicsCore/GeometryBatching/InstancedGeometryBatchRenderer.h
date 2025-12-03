#pragma once

#include <glm/mat4x4.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Graphics {
class CameraSceneProxy;
class ActiveBindedState;
} // namespace Graphics

namespace Graphics::GeometryBatching {
class InstancedGeometryBatchProxy;

class InstancedGeometryBatchRenderer {
    std::unordered_map<std::string, std::shared_ptr<InstancedGeometryBatchProxy>> mBatchProxies;

public:
    bool TryToAddBatchProxy(const std::shared_ptr<InstancedGeometryBatchProxy>& batch);

    bool CheckIfBatchProxyExists(const std::string& batchKey) const;

    std::shared_ptr<InstancedGeometryBatchProxy> GetBatchProxy(const std::string& batchKey) const;

    void RemoveBatchProxy(const std::string& batchKey);

    void RenderAllBatches(
        const std::shared_ptr<::Graphics::CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState);

    void UpdateBatchInstancesData(const std::unordered_map<std::string, std::vector<int32_t>>& data);

    void CleanUp();
};
} // namespace Graphics::GeometryBatching
