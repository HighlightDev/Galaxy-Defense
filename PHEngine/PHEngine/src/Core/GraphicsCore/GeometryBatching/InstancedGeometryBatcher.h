#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/mat4x4.hpp>

namespace Graphics
{
    class CameraSceneProxy;
}

namespace Graphics::GeometryBatching
{
    class InstancedGeometryBatch;

    class InstancedGeometryBatcher
    {
        std::unordered_map<std::string, std::shared_ptr<InstancedGeometryBatch>> mBatches;

    public:
        bool TryToAddBatch(const std::shared_ptr<InstancedGeometryBatch> &batch);

        bool CheckIfBatchExists(const std::string &batchKey) const;

        std::shared_ptr<InstancedGeometryBatch> GetBatch(const std::string &batchKey) const;

        void RemoveBatch(const std::string &batchKey);

        void RenderAllBatches(const std::shared_ptr<::Graphics::CameraSceneProxy> &cameraSceneProxy,
                              const glm::mat4 &viewMatrix,
                              const glm::mat4 &projectionMatrix);
    };
}
