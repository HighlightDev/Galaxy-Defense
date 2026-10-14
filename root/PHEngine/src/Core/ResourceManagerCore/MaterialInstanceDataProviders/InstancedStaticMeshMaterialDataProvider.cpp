#include "InstancedStaticMeshMaterialDataProvider.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchHolder.h"

using namespace EngineCore;

namespace Resources {
void InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider()
{
    if (const auto& sceneSp = GetSceneWp().lock()) {
        const auto& batchKey = GetBatchKey();
        const auto& batchHolder = sceneSp->GetInstancedGeometryBatchHolder();
        const auto& batchSp = batchHolder->GetBatch(batchKey);
        ext_assert(batchSp, "InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider: Batch not found: " + batchKey);

        mRenderInstanceId = batchSp->GetRenderInstanceId(GetInstanceProxyId());
        mIsInstanceActive = mRenderInstanceId != -1;
    } else {
        mRenderInstanceId = -1;
        mIsInstanceActive = false;
    }
}
} // namespace Resources
