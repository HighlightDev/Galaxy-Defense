#include "InstancedStaticMeshMaterialDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatcher.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"
#include "Core/CommonCore/Assertion.h"

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Graphics::GeometryBatching;

namespace Resources
{
    void InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider()
    {
        static const uint64_t functionId = Hash("InstancedStaticMeshMaterialDataProvider::UpdateInstancedDataProvider");
        if (!mIsBeingUpdatedDataOnRenderThread.load(std::memory_order::memory_order_seq_cst))
        {
            if (const auto &sceneSp = GetSceneWp().lock())
            {
                if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    mIsBeingUpdatedDataOnRenderThread.store(true, std::memory_order::memory_order_seq_cst);
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetInstanceObjectId(), functionId, [this, sceneRendererSp]()
                                                                                        {
                    const auto &instancedGeometryBatcherSp = sceneRendererSp->GetInstancedGeometryBatcher();
                    if (const auto &batchSp = instancedGeometryBatcherSp->GetBatch(GetBatchKey()))
                    {
                        mInstanceId = batchSp->GetInstanceId(GetInstanceProxyId());
                        mIsInstanceActive = mInstanceId != -1;
                    }
                    mIsBeingUpdatedDataOnRenderThread.store(false, std::memory_order::memory_order_seq_cst); });
                }
            }
        }
    }
}
