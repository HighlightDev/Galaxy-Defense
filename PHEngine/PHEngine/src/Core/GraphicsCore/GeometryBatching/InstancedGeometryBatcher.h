#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Graphics::GeometryBatching
{
    class InstancedGeometryBatch;

    class InstancedGeometryBatcher
    {
        std::unordered_map<std::string, std::shared_ptr<InstancedGeometryBatch>> mBatches;

    public:
        bool TryToAddBatch(const std::string &batchKey, const std::shared_ptr<InstancedGeometryBatch> &batch);

        void RemoveBatch(const std::string &batchKey);
    };
}
