#include "InstancedGeometryBatcher.h"
#include "InstancedGeometryBatch.h"
#include "Core/CommonCore/Assertion.h"

namespace Graphics::GeometryBatching
{
    bool InstancedGeometryBatcher::TryToAddBatch(const std::string &batchKey, const std::shared_ptr<InstancedGeometryBatch> &batch)
    {
        if (!mBatches.count(batchKey))
        {
            mBatches.emplace(batchKey, batch);
            return true;
        }
        return false;
    }

    void InstancedGeometryBatcher::RemoveBatch(const std::string &batchKey)
    {
        ext_assert(mBatches.count(batchKey), "Batch with sought batch name doesn't exist.");
        mBatches.erase(batchKey);
    }
}
