#include "InstancedGeometryBatcher.h"
#include "InstancedGeometryBatch.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/CommonCore/Assertion.h"

using namespace Graphics;

namespace Graphics::GeometryBatching
{
    bool InstancedGeometryBatcher::TryToAddBatch(const std::shared_ptr<InstancedGeometryBatch> &batch)
    {
        const auto &batchKey = batch->GetBatchKey();
        assert(batchKey != "");
        if (!mBatches.count(batchKey))
        {
            mBatches.emplace(batchKey, batch);
            return true;
        }
        return false;
    }

    bool InstancedGeometryBatcher::CheckIfBatchExists(const std::string &batchKey) const
    {
        return mBatches.count(batchKey) > 0;
    }

    std::shared_ptr<InstancedGeometryBatch> InstancedGeometryBatcher::GetBatch(const std::string &batchKey) const
    {
        assert(batchKey != "");
        if (mBatches.count(batchKey))
        {
            return mBatches.at(batchKey);
        }
        return nullptr;
    }

    void InstancedGeometryBatcher::RemoveBatch(const std::string &batchKey)
    {
        ext_assert(mBatches.count(batchKey), "Batch with sought batch name doesn't exist.");
        mBatches.erase(batchKey);
    }

    void InstancedGeometryBatcher::RenderAllBatches(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy,
                                                    const glm::mat4 &viewMatrix,
                                                    const glm::mat4 &projectionMatrix)
    {
        for (const auto &[key, batchSp] : mBatches)
        {
            batchSp->Render(cameraSceneProxy, viewMatrix, projectionMatrix);
        }
    }
}
